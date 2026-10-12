"""Embed validated launcher assets without additional runtime dependencies."""
import json
from pathlib import Path
root = Path(__file__).resolve().parents[1]
param = (root / 'assets/param.json').read_bytes()
config = json.loads(param)
assert config['titleId'] == 'BDUL00001'
assert config['deeplinkUri'] == 'https://www.baidu.com/'
icon = (root / 'assets/icon0.png').read_bytes()
assert icon[:8] == b'\x89PNG\r\n\x1a\n'
assert int.from_bytes(icon[16:20], 'big') == 512
assert int.from_bytes(icon[20:24], 'big') == 512
out = root / 'build/assets.h'
out.parent.mkdir(exist_ok=True)
with out.open('w', newline='\n') as f:
    for name, data in [('param_json', param), ('icon_png', icon)]:
        f.write(f'static const unsigned char {name}[] = {{\n')
        for i in range(0, len(data), 24):
            f.write(','.join(str(b) for b in data[i:i+24]) + ',\n')
        f.write('};\n')
