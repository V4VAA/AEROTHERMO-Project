[CmdletBinding()]
param(
    [ValidateSet('AE4080', 'HW1', 'All')]
    [string]$Target = 'AE4080',
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    [switch]$Run
)

$ErrorActionPreference = 'Stop'
if ($Run -and $Target -eq 'All') {
    throw 'Choose AE4080 or HW1 when using -Run.'
}

# Prefer the bundled CMake, since an older CMake on PATH may lack the VS 2026 generator.
$vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$cmakePath = $null
if (Test-Path -LiteralPath $vswherePath) {
    $vsInstallPath = & $vswherePath -latest -version '[18.0,19.0)' -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vsInstallPath) { throw 'Visual Studio 2026 C++ Build Tools were not found.' }
    $cmakePath = Join-Path $vsInstallPath 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
}
if (-not $cmakePath -or -not (Test-Path -LiteralPath $cmakePath)) {
    $cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
    if (-not $cmakeCommand) {
        throw 'Install Visual Studio 2026 C++ Build Tools with CMake tools, or add CMake 4.2 or newer to PATH.'
    }
    $cmakePath = $cmakeCommand.Source
}

Push-Location -LiteralPath $PSScriptRoot
try {
    & $cmakePath --preset msvc
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    $buildArguments = @('--build', '--preset', $Configuration.ToLowerInvariant(), '--parallel', '1')
    if ($Target -ne 'All') { $buildArguments += @('--target', $Target) }
    # These small targets need no persistent MSBuild worker pool.
    $buildArguments += @('--', '/nr:false')
    & $cmakePath @buildArguments
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    if ($Run) {
        # EquilFlow reads GasModels/ and Species/ relative to the working directory.
        & (Join-Path $PSScriptRoot "build\cmake-vs2026\bin\$Configuration\$Target.exe")
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
} finally {
    Pop-Location
}
