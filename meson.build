project('curves', 'cpp')

src = ['src/main.cpp', 'src/bezier.cpp', 'src/utils.cpp']

sfmldep = dependency('sfml-graphics')
executable('curves', src, dependencies: sfmldep, cpp_args: '--std=c++2a')
