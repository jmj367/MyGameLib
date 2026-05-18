# メッシュ

```json
{
    "version": バージョン数(int)(今のところ意味なし),
    "vertexformat": "PosNormTex" or "PosNormSkinTex",
    "shader": "shaderName",
    "textures": [
        "png Path",
        "png Path"
    ],
    "specularPower": 鏡面反射の強さ(float),
    "vertices": [
        [　位置.x, y, z, 法線.x, y, z, (PosNormSkinTexの場合: ボーンのインデックス1, 2, 3, 4, ウェイト1, 2, 3, 4,) テクスチャ座標.u, v ],
        [...],
        .
        .
        .
    ]
}
```

# スケルトン

```json
{
    "version": バージョン数(int)(今のところ意味なし),
    "bonecount": ボーン数(int),
    "bones": [ 各ボーン(順不同)
        {
            "name": "hip",
            "parent": 親のインデックス,
            "bindpose": {
                "rot": [
                    x,
                    y,
                    z,
                    w
                ],
                "trans": [
                    x,
                    y,
                    z
                ]
            }
        },
        {
            ...
        },
        .
        .
        .
    ]
}
```