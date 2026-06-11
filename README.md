# [MyGameLib]

C++で書かれたゲーム用ライブラリ  
制作途中です

## 特徴

### 入力を簡単にバインド

InputSystemのLoadInputBindingsでキーボード・マウス・コントローラーの入力をCSVからバインドすることができます

使用例

```cpp
InputSystem inputSystem;
inputSystem.LoadInputBindings("binding_file.csv");

InputState inputState = inputSystem.GetState();
if (inputState.GetActionPressed("Jump"))
{
    // Jump
}
```

CSVの構造

```csv
// 先頭一行目は読み込まれない
Action  , Device    , EButton
Jump    , Keyborad  , ESpace
Jump    , Mouse     , ELeft
Jump    , Controller, EA
```

### 直感的なJSON読込

JsonDataを用いてJSONファイルを直感的に扱うことができます

使用例

```cpp
JsonData jsonData;
jsonData.LoadJSON("file.json");

JsonObject root = jsonData.GetObject();
int number;
if (!root.Get("number", number))
{
    return false;
}
```

### ポストプロセスの追加が容易

RendererのDrawPostProcessにシェーダーを渡すだけでポストプロセスを追加できます

使用例

```cpp
Renderer renderer;
Renderer::ResourceID shaderID
if (!renderer.GetShader("gl_post.vert", "gl_post.frag", shaderID))
{
    return false;
}
renderer.DrawPostProcess(shaderID);
```

## 使用ライブラリ
- GLEW 2.3.1
- OpenGL 3.3.0
- SDL 2.32.4
- stb_image 2.30
- RapidJSON 1.1.0