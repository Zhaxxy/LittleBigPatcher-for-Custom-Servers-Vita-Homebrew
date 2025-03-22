from pathlib import Path

MAIN_PATH = Path('src/lua-5.4.7/src')

for x in Path('src').iterdir():
    if x.is_file() and x.suffix.lower() == '.c':
        print((MAIN_PATH / x.name).as_posix())