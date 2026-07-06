# shared/

Copia aquí `stb_image.h` desde tu carpeta `shared/` de `cg-labs` (la que ya
usaste en el lab de texturas). Es header-only, una sola línea de include:

```
shared/
└── stb_image.h
```

Si en algún momento quieres reemplazar las primitivas procedurales de
`src/Primitivas.h/.cpp` por tus modelos `.obj` reales (por ejemplo un barco
o faro más detallado), copia también `OBJLoader.h` aquí y ajusta
`Barco.cpp`/`Faro.cpp` para cargar la malla en vez de generarla.
