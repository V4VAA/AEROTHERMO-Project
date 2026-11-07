# AEROTHERMO Project

The AE4080 driver computes freestream properties and normal and oblique
shock results using EquilFlow.

## Set up on a new Windows PC

The EquilFlow binaries require a 64-bit Windows build with MSVC. In Visual
Studio Installer, install **Visual Studio 2022** or **Build Tools for Visual
Studio 2022** with the **Desktop development with C++** workload. Include
the **MSVC v143 x64/x86 build tools**, a **Windows 10 or 11 SDK**, and
**C++ CMake tools for Windows**. The project needs CMake 3.21 or newer.
See [Microsoft's Build Tools component list](https://learn.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?view=vs-2022).

Install [Git for Windows](https://git-scm.com/install/windows), then open
PowerShell in the directory where you want the project and run:

```powershell
git clone https://github.com/V4VAA/AEROTHERMO-Project.git
cd AEROTHERMO-Project
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 -Run
```

The helper uses the CMake installation on PATH or the copy installed with
Visual Studio. The first run builds the Release executable and prints the
freestream and shock results. The execution policy flag applies to that
PowerShell process.

## Repository layout

| Path | Contents |
| --- | --- |
| `src/` | Project driver, atmosphere and shock routines, and the cone aerodynamics routine. The cone routine is currently outside the executable targets. |
| `EquilFlow/` | Course-supplied Phase 2 header and precompiled Debug and Release libraries for MSVC x64. |
| `GasModels/`, `Species/` | EquilFlow runtime data. |
| `examples/HW1.cpp` | Standalone EquilFlow composition example. |

## Build options

From PowerShell at the repository root:

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

If your PowerShell execution policy blocks direct script execution, use
the quick-start command above with the same target or configuration flags.

With CMake on PATH, you can also configure and build directly:

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
