#!/usr/bin/env ruby

# bitplane_maker.rb
#
# Project : jsTetris 
# Author  : Felix Naredi
# Date    : 2017-12-02
#

class String
  
  def to_a()
    ret = []
    self.each_char { |c| ret += [c] }
    ret
  end
  
end

def hexf(n)
  ("0x%08X" % n).gsub '.', 'F'
end

SHAPES = [
  [
    [
      " 11 ",
      " 11 ",
      "    ",
      "    "
    ]
  ],
  [
    [
      " 1  ",
      " 1  ",
      " 1  ",
      " 1  "
    ],
    [
      "    ",
      "1111",
      "    ",
      "    "
    ]
  ],
  [
    [
      " 1  ",
      " 11 ",
      "  1 ",
      "    "
    ],
    [
      "  11",
      " 11 ",
      "    ",
      "    "
    ]
  ],
  [
    [
      "  1 ",
      " 11 ",
      " 1  ",
      "    "
    ],
    [
      " 11 ",
      "  11",
      "    ",
      "    "
    ]
  ],
  [
    [
      " 1  ",
      " 1  ",
      " 11 ",
      "    "
    ],
    [
      "    ",
      "111 ",
      "1   ",
      "    "
    ],
    [
      "11  ",
      " 1  ",
      " 1  ",
      "    "
    ],
    [
      "  1 ",
      "111 ",
      "    ",
      "    "
    ],
  ],
  [
    [
      "  1 ",
      "  1 ",
      " 11 ",
      "    "
    ],
    [
      " 1  ",
      " 111",
      "    ",
      "    "
    ],
    [
      "  11",
      "  1 ",
      "  1 ",
      "    "
    ],
    [
      "    ",
      " 111",
      "   1",
      "    "
    ],
  ],
  [
    [
      " 1  ",
      " 11 ",
      " 1  ",
      "    ",
    ],
    [
      "    ",
      "111 ",
      " 1  ",
      "    "
    ],
    [
      " 1  ",
      "11  ",
      " 1  ",
      "    "
    ],
    [
      " 1  ",
      "111 ",
      "    ",
      "    "
    ],
  ],
]

def points_from_rows(rows)
  def points_from_row(row)
    ret = []
    row.to_a.each_with_index { |c,i| ret.push(i) if c == '1' }
    ret
  end
  ret = []
  rows.reverse.map { |row| points_from_row(row) }.each_with_index do |e,i|
    e.each { |px| ret.push({:x => px, :y => i}) }
  end
  ret
end

def print_shape_points
  puts "static const jsVec2i shapeVerticies[] = {"
  SHAPES.flatten(1).each do |shape|
    print "\t{ "
    print points_from_rows(shape).map { |p| "{#{p[:x]}, #{p[:y]}}" }.join(", ")
    puts " },"
  end
  puts "};"
end

def print_rot_ranges(varname = 'rotRanges', datatype = 'int')
  puts "static const #{datatype} #{varname}[]#{datatype == 'int' ? '[2]' : ''} = {"
  print "\t"
  index = 0
  SHAPES.each { |shape|
    print "{#{index}, #{index + shape.length - 1}}, "
    index += shape.length
  }
  puts "\n};"
end

def shape_verticies

  def rect(points)
    xs = points.reduce([]) { |s, p| s.push p[:x] }
    x_min = xs.min
    x_max = xs.max
    
    ys = points.reduce([]) { |s, p| s.push p[:y] }
    y_min = ys.min
    y_max = ys.max

    {
      :w => x_max - x_min + 1,
      :h => y_max - y_min + 1,
      :x => x_min,
      :y => y_min
    }
  end
  
  forms = [
    	'JS_SHAPE_FORMATION_O',
	'JS_SHAPE_FORMATION_I',
	'JS_SHAPE_FORMATION_S',
	'JS_SHAPE_FORMATION_Z',
	'JS_SHAPE_FORMATION_L',
	'JS_SHAPE_FORMATION_J',
	'JS_SHAPE_FORMATION_T',
  ]
  puts 'static const jsShape shape_verticies[] = {'
  SHAPES.each { |form|
    fn = forms.shift
    form.each { |shape|
      ps = points_from_rows shape
      r = rect ps
      print "\t{ {#{5 - r[:x] - r[:w] / 2}, #{20 - r[:y] - r[:h]}}, "
      print "JS_SHAPE_VERTICIES(#{fn}, "
      print ps.map { |p| "#{p[:x]}, #{p[:y]}"}.join ', '
      puts ") },"
    }
  }
  puts '};'
end
