import shutil
import os
import subprocess
import sys

app_path  = 'Data/Binaries/glslangValidator'
app_flags = '-V100 --target-env vulkan1.3 -g0'
src_path  = 'Data/Shaders'

def compile(app_path, app_flags, src_path, dst_path):
	for src in os.listdir(src_path):
		args = [f'{app_path}', app_flags, os.path.join(src_path, src), '-o', os.path.join(dst_path, f'{src}.spv')]
		subprocess.run(args)

def main():
	if len(sys.argv) <= 1:
		print(f'python compile_shaders.py [build]')
		return

	build = sys.argv[1].lower()
	dst_path = ''
	if 'debug' == build:
		dst_path = 'Build/Debug'
	elif 'release' == build:
		dst_path = 'Build/Release'
	else:
		print(f'build `{build}` not recognized')
	
	if not os.path.exists(dst_path):
		os.makedirs(dst_path)

	compile(app_path, app_flags, src_path, dst_path)


if __name__ == '__main__':
	main() 
