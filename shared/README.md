# shared/

Copiamos aquí `stb_image.h` desde nuestra carpeta `shared/` de `cg-labs` (la que ya
usamos en el lab de texturas). Es header-only, una sola línea de include:

```
shared/
└── stb_image.h
```

Si en algún momento queremos reemplazar las primitivas procedurales de
`src/Primitivas.h/.cpp` por modelos `.obj` reales (por ejemplo un barco
o faro más detallado), copiamos también `OBJLoader.h` aquí y ajustamos
`Barco.cpp`/`Faro.cpp` para cargar la malla en vez de generarla.
