import gdstk
import pathlib
import cairosvg

gds = sorted(pathlib.Path('runs').glob('run2/results/final/gds/*.gds'))
library = gdstk.read_gds(gds[-1])
top_cells = library.top_level()
top_cells[0].write_svg('gds_render.svg')

cairosvg.svg2png(url='gds_render.svg', write_to='gds_render.png')