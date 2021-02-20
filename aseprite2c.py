from pydantic import BaseModel
import click
from collections import defaultdict
import json
import re
from PIL import Image
import struct
from typing import List, Dict

class Rect(BaseModel):
	x: int
	y: int
	w: int
	h: int

class Size(BaseModel):
	w: int
	h: int

class SpriteSheet(BaseModel):
	class Frame(BaseModel):
		frame: Rect
		rotated: bool
		trimmed: bool
		spriteSourceSize: Rect
		sourceSize: Size
		duration: int
	frames: Dict[str, Frame]

	class Meta(BaseModel):
		app: str
		version: str
		image: str
		format: str
		size: Size
		scale: str
		frameTags: List[str]

		class Layer(BaseModel):
			name: str
			opacity: int
			blendMode: str

		layers: List[Layer]
		slices: list
	meta: Meta


@click.command()
@click.argument('json-file', type=click.File('r'), required=True)
def convert(json_file: click.File):
	"""Convert Aseprite JSON spritesheet data to embedded C code."""
	with json_file as f:
		spritesheet = SpriteSheet.parse_obj(json.load(f))

	animation_name = spritesheet.meta.image.rsplit(".", 1)[0]
	common_header_path = '../image_common.h'
	header_path = 'data/{}.h'.format(animation_name)
	cpp_path = 'data/{}.cpp'.format(animation_name)

	# convert image to a c array
	img_data = []
	with Image.open(spritesheet.meta.image).convert('1') as image:
		# convert to 1bpp data
		buff = 0
		i = 0
		for y in range(image.height):
			for x in range(image.width):
				val = image.getpixel((x, y))
				buff |= (1 if val else 0) << i
				i += 1
				if i == 8:
					img_data.append(buff)
					buff = 0
					i = 0

			# pad with 0s to 8 if width is not 8 aligned
			if i != 0:
				img_data.append(buff)
				buff = 0
				i = 0
	
	with open(header_path, 'w') as f:
		f.write(f"""#pragma once
#include "{common_header_path}"

extern const Animation animation_{animation_name};
""")

	with open(cpp_path, 'w') as f:
		f.write(f'#include "{animation_name}.h"\n\n')

		# write image data
		f.write("static const uint8_t _image_data[{}] = {{\n  ".format(len(img_data)))
		for i, byte in enumerate(img_data):
			if (i > 0) and (i % 18 == 0):
				f.write("\n  ");
			f.write("0x{:02X},".format(byte))
		f.write("\n};\n")

		# write frame data
		frame_data = defaultdict(dict)
		for frame_name, frame in spritesheet.frames.items():
			layer_name, frame_num = re.match(r".+ \((.+)\) (\d+)", frame_name).groups()
			frame_data[frame_num]["duration_ms"] = frame.duration
			frame_data[frame_num].setdefault("layers", {})[layer_name] = {
				"x": frame.frame.x,
				"y": frame.frame.y,
				"width": frame.frame.w,
				"height": frame.frame.h,
				"dest_offset_x": frame.spriteSourceSize.x,
				"dest_offset_y": frame.spriteSourceSize.y,
			}
		
		f.write("\nstatic const Animation::Frame _frames[] = {\n")
		for frame_num, frame in frame_data.items():
			f.write("  { {\n")
			for layer_num, layer_name in enumerate([l.name for l in spritesheet.meta.layers]):
				layer_data = frame["layers"][layer_name]
				x = layer_data["x"]
				y = layer_data["y"]
				width = layer_data["width"]
				height = layer_data["height"]
				offset_x = layer_data["dest_offset_x"]
				offset_y = layer_data["dest_offset_y"]
				f.write(f"    {{{x}, {y}, {width}, {height}, {offset_x}, {offset_y}}},\n")
			f.write("  },\n")

			duration = frame["duration_ms"]
			f.write(f"  {duration} }},\n")
		f.write("\n};\n")
		

		# finally, write animation struct
		padded_width = ((spritesheet.meta.size.w + 7) // 8) * 8
		f.write(f"""
const Animation animation_{animation_name} = {{
  "{animation_name}", {len(spritesheet.meta.layers)}, {len(frame_data)},
  _image_data, {padded_width}, {spritesheet.meta.size.h},
  _frames
}};
""")



if __name__ == '__main__':
    convert()