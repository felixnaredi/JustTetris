#!/usr/bin/env ruby
#
# Filename: shape_data.rb
# Created: 2018-03-12 22:45:12 +0100
# Author: Felix Nared
#

require 'json'

def points_from_rows(rs)
  rs_rev = rs.reverse

  (0...rs_rev.length).reduce([]) { |s, y|
    (0...rs_rev[y].length).each { |x|
      s.push ({:x => x, :y => y}) if rs_rev[y][x]
    }
    s
  }
end

def shape_verticies

  shapes = (JSON.parse (open 'data.json').read)['shapes']

  # Names of parameters in C output.
  type = '__jsShapeData'
  vname = 'shape_data'
  macro = 'JS_SHAPE_DATA'
  forms = [
    'jsShapeFormationO',
    'jsShapeFormationI',
    'jsShapeFormationS',
    'jsShapeFormationZ',
    'jsShapeFormationL',
    'jsShapeFormationJ',
    'jsShapeFormationT',
  ]

  def rect(points)
    xs = points.map { |p| p[:x] }
    x_min = xs.min
    x_max = xs.max

    ys = points.map { |p| p[:y] }
    y_min = ys.min
    y_max = ys.max

    {
      :w => x_max - x_min + 1,
      :h => y_max - y_min + 1,
      :x => x_min,
      :y => y_min
    }
  end

  puts "static const #{type} #{vname}[] = {"
  shapes.each { |form|
    fn = forms.shift
    form.each { |shape|
      ps = points_from_rows shape
      r = rect ps
      print "\t{ {#{5 - r[:x] - r[:w] / 2}, #{20 - r[:y] - r[:h]}}, "
      print "#{macro}(#{fn}, "
      print ps.map { |p| "#{p[:x]}, #{p[:y]}"}.join ', '
      puts ") },"
    }
  }
  puts '};'
end

shape_verticies
