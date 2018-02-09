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
  ("0x%08X" %n).gsub '.', 'F'
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

def bits_from_rows(rows)
  def bits_from_row(row)
    row.to_a.inject({:value => 0, :index => 0}) do |s,c|
      s[:value] |= (1 << 31) >> s[:index] if c == '1'
      s[:index] += 1
      s
    end[:value]
  end
  a = Array.new(4 - rows.count, 0)
  (a + rows.map { |row| bits_from_row(row) }).reverse
end

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

def print_shape_bitplanes()
  puts "static const int shapeBitplanes[19][4] = {"
  SHAPES.flatten(1).each do |shape|
    print "\t{"
    print bits_from_rows(shape).each.map { |row| hexf(row) }.join(", ")
    puts "},"
  end
  puts "};"
end

def print_shape_points
  puts "static const __JSTPoint shapePoints[] = {"
  SHAPES.flatten(1).each do |shape|
    print "\t{ "
    print points_from_rows(shape).map { |p| "{#{p[:x]}, #{p[:y]}}" }.join(", ")
    puts " },"
  end
  puts "};"
end

