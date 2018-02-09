#!/usr/bin/env ruby

ex_struct_01 =
"typedef struct __JSTGrid\n" +
"{\n" +
"	__JST_PRIVATE__ int x;\n" +
"	__JST_PRIVATE__ int y;\n" +
"	int width;\n" +
"	int height;\n" +
"	JSTGridItem *items;\n" +
"	int length;\n" +
"	__JST_PRIVATE__ __JSTBitplane *bitplane;\n" +
"} __JSTGrid;"

ex_struct_02 =
"typedef struct __JSTShape\n" \
"{\n" \
"	int x;\n" \
"	int y;\n" \
"	int width;\n" \
"	int height;\n" \
"	__JST_PRIVATE__ JSTGridItem *items;\n" \
"	__JST_PRIVATE__ int length;\n" \
"	JSTGrid *board;\n" \
"	__JST_PRIVATE__ int index;\n" \
"	__JST_PRIVATE__ int minIndex;\n" \
"	__JST_PRIVATE__ int maxIndex;\n" \
"} __JSTShape;"

jst_bitplane =
"typedef struct __JSTBitplane\n" \
"{\n" \
"	__JST_PRIVATE__ int *rows;\n" \
"	__JST_PRIVATE__ int width;\n" \
"	__JST_PRIVATE__ int height;\n" \
"	__JST_PRIVATE__ int offset;\n" \
"	__JST_PRIVATE__ int fill;\n" \
"} __JSTBitplane;\n"

class Field
  @is_private
  @is_reserved
  @type
  @title
  
  def initialize(str)
    res = /\s*(__(.*)__\s+)?(\w*)\s+(\**)(\w*);/.match(str)
    return nil unless res
    @is_private = res[2] == 'JST_PRIVATE'
    @is_reserved = res[2] == 'JST_RESERVED'
    @type = res[3] + " " + (res[4].nil? ? "" : res[4])
    @title = res[5]
  end

  def pointer?
    /\*/.match(@type)
  end

  def incomplete?
    [@is_private, @is_reserved, @type, @title].map { |e| e.nil? }.include?(true)
  end

  def as_public_str(count)
    type = @type
    comment = nil
    if @is_private
      type = 'void *' if pointer?
      title = "__private_%02d" %count[:private]
      comment = "/* #{@title} */"
      count[:private] += 1
    else
      title = @title
    end
    "#{type}#{title};" + (comment.nil? ? '' : "\t#{comment}")
  end
  
end

class CStruct
  attr_reader :fields
  
  @title
  @fields

  def initialize(str)
    @title = /(typedef\s+)?\s*struct\s+(\w*)/.match(str)[2]
    fields = []
    str.each_line do |line|
      break if line.include? '}'
      field = Field.new(line)
      fields.push(field) unless field.incomplete?
    end
    @fields = fields.compact
  end

  def as_public_str
    count = {:private => 1, :reserved => 1}
    title = /(__)?(\w*)/.match(@title)[2]
    puts "typedef struct #{title}"
    puts '{'
    @fields.each { |f| puts "\t#{f.as_public_str(count)}" }
    puts "} #{title};"
  end
  
end

