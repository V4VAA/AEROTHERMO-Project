# AE4080 C++

The AE4080 driver computes freestream properties and normal and oblique
shock results using EquilFlow. The repository is organized as follows.

| Path | Contents |
| --- | --- |
| `src/` | Project driver, atmosphere and shock routines, and the cone aerodynamics routine. The cone routine is currently outside the executable targets. |
| `EquilFlow/` | Course-supplied Phase 2 header and precompiled Debug and Release libraries for MSVC x64. |
| `GasModels/`, `Species/` | EquilFlow runtime data. |
| `examples/HW1.cpp` | Standalone EquilFlow composition example. |

## Build and run

Requires Visual Studio 2022 C++ Build Tools (MSVC x64 and a Windows SDK),
and CMake 3.21 or newer. The PowerShell helper finds CMake on PATH or in
the Visual Studio installation. No developer shell is needed.

From PowerShell in this folder:

```powershell
.\build.ps1 -Run
.\build.ps1 -Target HW1 -Run
.\build.ps1 -Configuration Debug -Run
.\build.ps1 -Target All
```

The default target is `AE4080`, and the default configuration is `Release`.
Every run through the helper first builds the selected target and stops if
configuration or compilation fails. CMake tracks source/header dependencies,
so unchanged files are not recompiled. `All` builds both executables.

If local PowerShell execution policy blocks the script, invoke it with:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 -Run
```

When CMake is on PATH, the equivalent commands are:

```powershell
cmake --preset msvc
cmake --build --preset release --target AE4080 --parallel 1 -- /nr:false
.\build\cmake\bin\Release\AE4080.exe
```

Run executables directly from the repository root because EquilFlow reads
`GasModels/` and `Species/` using relative paths.
It can also write `jacobi_out.dat` there.

## EquilFlow compatibility

Release uses `EquilFlow/Release/EquilFlowLib.lib` with `/MD`; Debug uses
`EquilFlow/Debug/EquilFlowLib.lib` with `/MDd`. The course package
supplies the binaries without library source or debug symbols. CMake
suppresses the corresponding LNK4099 warning.
