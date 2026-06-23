#include "Game.h"
#include "Scene.h"
#include "SDL2Window.h"
#include "WindowEvent.h"

Game::Game()
    : mGameState(EGameplay), mIsDebug(false)
{
}

bool Game::Initialize(WindowBackendType windowBackend, GraphicsAPI graphicsAPI)
{
    switch (windowBackend)
    {
    case WindowBackendType::SDL2:
        // SDL2Windowの初期化
        mWindow = std::make_unique<SDL2Window>();
        if (!mWindow->Initialize())
        {
            return false;
        }
        break;
    default:
        return false;
    }

    // レンダラーの初期化
    if (!mRenderer.Initialize(
            mWindow->GetWindowHandle(),
            mWindow->GetScreenWidth(),
            mWindow->GetScreenHeight(),
            graphicsAPI))
    {
        return false;
    }

    // インプットシステムの初期化
    if (!mInputSystem.Initialize())
    {
        return false;
    }

    // シーンのロード予約
    mEventBus.Subscribe<EventLoadScene>([this](const EventLoadScene &event)
    {
        ReserveSceneLoad({ event.SceneName, event.IsActive });
    });

    // シーン起動のイベントを購読
    mEventBus.Subscribe<EventActivateScene>([this](const EventActivateScene &event)
    {
        ReserveSceneActivation(event.SceneName);
    });

    // フレーム開始時の時間を記録
    mLastFrameTime = Clock::now();

    return true;
}

void Game::RunLoop()
{
    while (mGameState != EQuit)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
        InnactivateScenes();
        UnloadScenes();
        LoadScenes();
        ActivateScenes();
    }
}

void Game::Shutdown()
{
    // レンダラーのシャットダウン
    mRenderer.Shutdown();

    // インプットシステムのシャットダウン
    mInputSystem.Shutdown();

    // ウィンドウの破棄
    mWindow->Destroy();
}

void Game::ReserveSceneActivation(const std::string &sceneName)
{
    mReservedSceneActivations.push_back(sceneName);
}

void Game::ProcessInput()
{
    mInputSystem.PreparePollEvent();

    WindowEvent windowEvent;
    while (mWindow->PollWindowEvents(windowEvent))
    {
        // ウィンドウイベントの処理
        switch (windowEvent.EventType)
        {
        case WindowEventType::Quit:
            mGameState = EQuit;
            break;
        case WindowEventType::MouseWheel:
            mInputSystem.UpdateMouseWheelDelta(
                static_cast<float>(windowEvent.MouseWheel.WheelX),
                static_cast<float>(windowEvent.MouseWheel.WheelY));
            break;
        case WindowEventType::ControllerAdded:
            mInputSystem.AddController(windowEvent.Controller.ControllerID);
            break;
        case WindowEventType::ControllerRemoved:
            mInputSystem.RemoveController(windowEvent.Controller.ControllerID);
            break;
        }
    }

    mInputSystem.Update();
}

void Game::UpdateGame()
{
    // deltaTimeの計算
    TimePoint currentTime = Clock::now();
    std::chrono::duration<float> deltaTimeDuration = currentTime - mLastFrameTime;
    float deltaTime = deltaTimeDuration.count();
    mLastFrameTime = currentTime;

    // deltaTimeが大きすぎる場合は補正する(デバッグ用)
    // HACK: 暫定的な処理
    if(deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    if (mGameState == EGameplay)
    {
        // シーンの更新
        for (Scene *scene : mActiveScenes)
        {
            scene->Update({ deltaTime });
        }
    }
}

void Game::GenerateOutput()
{
    // シーンの描画
    for (Scene *scene : mActiveScenes)
    {
        scene->Draw();
    }

    mRenderer.Draw(mWindow->GetWindowHandle());
}

void Game::InnactivateScenes()
{
    for (Scene *scene : mActiveScenes)
    {
        if (scene->GetStatus() == Scene::Status::EExiting)
        {
            scene->OnExit();
            scene->SetStatus(Scene::Status::EInactive);
            mActiveScenes.erase(std::remove(mActiveScenes.begin(), mActiveScenes.end(), scene), mActiveScenes.end());
        }
    }
}

void Game::UnloadScenes()
{
    for (auto iter = mScenes.begin(); iter != mScenes.end();)
    {
        if (iter->second->GetStatus() == Scene::Status::EUnloading)
        {
            iter = mScenes.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void Game::LoadScenes()
{
    for (const PendingSceneLoad &pendingScene : mPendingSceneLoads)
    {
        // シーンのロード
        std::unique_ptr<Scene> newScene = std::make_unique<Scene>();
        mScenes[pendingScene.SceneName] = std::move(newScene);

        // ロード後すぐにアクティブにする場合はアクティベーション予約も行う
        if (pendingScene.IsActive)
        {
            ReserveSceneActivation(pendingScene.SceneName);
        }
    }

    mPendingSceneLoads.clear();
}

void Game::ActivateScenes()
{
    for (const std::string &sceneName : mReservedSceneActivations)
    {
        auto iter = mScenes.find(sceneName);
        if (iter != mScenes.end())
        {
            Scene *scene = iter->second.get();
            if (scene->GetStatus() == Scene::Status::EInactive)
            {
                scene->OnEnter();
                mActiveScenes.push_back(scene);
                scene->SetStatus(Scene::Status::EActive);
            }
        }
    }

    mReservedSceneActivations.clear();
}