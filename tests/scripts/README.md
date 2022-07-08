# Running the LittleFS tests

This requires Visual Studio 2022 with the Clang toolchain installed.

## Library test

```
cd /d X:\source\eve_apps\tests\scripts
set PATH=C:\Python38;%PATH%
python -m pip install --upgrade pip
python -m pip install toml
python test_littlefs.py
```

Cleanup the generated files from *"X:\source\eve_apps\dependencies\littlefs\tests"* afterwards.

## Prepare diff for re-merge

```
diff -u ../../dependencies/littlefs/scripts/test.py test_littlefs.py > test_littlefs.diff
```

## Emulator test

```
python test_littlefs.py --eve-platform=EVE_PLATFORM_BT8XXEMU --eve-graphics=EVE_GRAPHICS_VM816C
```

## Hardware test

```
python test_littlefs.py --eve-platform=EVE_PLATFORM_FT4222 --eve-graphics=EVE_GRAPHICS_VM816C
```

Run one specific test.

```
python test_littlefs.py --eve-platform=EVE_PLATFORM_FT4222 --eve-graphics=EVE_GRAPHICS_VM816C ../../dependencies/littlefs/tests/test_alloc.toml#9#1
```
