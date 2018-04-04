//
//  GridDescriptor.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/12/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import MetalKit


protocol RectangleCornerLayout {
  
  typealias Index = uint16
  
  static var bottomLeftCornerOffset: Index { get }
  static var bottomRightCornerOffset: Index { get }
  static var topLeftCornerOffset: Index { get }
  static var topRightCornerOffset: Index { get }
  
}


extension RectangleCornerLayout {
  
  static func triangleIndicies() -> [Index] {
    return [bottomLeftCornerOffset, topRightCornerOffset, topLeftCornerOffset,
            bottomLeftCornerOffset, bottomRightCornerOffset, topRightCornerOffset]
  }
  
  static func triangleIndicies(fromRectangle begin: Int) -> [Index] {
    return triangleIndicies().map { offset in return Index(begin) * 4 + offset }
  }
  
  static func lineIndicies() -> [Index] {
    return [bottomLeftCornerOffset, bottomRightCornerOffset,
            bottomRightCornerOffset, topRightCornerOffset,
            topRightCornerOffset, topLeftCornerOffset,
            topLeftCornerOffset, bottomLeftCornerOffset]
  }
  
  static func lineIndicies(fromRectangle begin: Int) -> [Index] {
    return lineIndicies().map { index in return Index(begin) * 4 + index }
  }
  
  static var metalIndexType: MTLIndexType { return MTLIndexType.uint16 }
  
}

/// A GridDescriptor containes everything needed to get graphical data,
/// such as verticies, indicies, etc. for drawing grid items at their
/// positions in the grid.
protocol GridDescriptor {
  
  associatedtype IndiciesLayout: RectangleCornerLayout
  
  typealias Matrix = float4x4
  
  var width: Int { get }
  var height: Int { get }
  
  var primitiveType: MTLPrimitiveType { get }
  
  func verticies(for blocks: [Block]) -> AnySequence<Vertex>
  
}


fileprivate extension GridDescriptor {
  
  typealias Index = IndiciesLayout.Index
  
  private static func scalar(width: Int, height: Int) -> Matrix {
    let w = 2 / Float(width)
    let h = 2 / Float(height)
    
    return Matrix(float4(w, 0, 0, 0),
                  float4(0, h, 0, 0),
                  float4(0, 0, 1, 0),
                  float4(0, 0, 0, 1))
  }
  
  private static var translate: Matrix {
    return Matrix(float4(1, 0, 0, -1),
                  float4(0, 1, 0, -1),
                  float4(0, 0, 1,  0),
                  float4(0, 0, 0,  1))
  }
  
  static func matrix(width: Int, height: Int) -> Matrix {
    return scalar(width: width, height: height) * translate
  }
  
  private static func triangleIndicies(width: Int, height: Int) -> [Index] {
    return Array((0..<(width * height)).map { (n) -> [Index] in
      return IndiciesLayout.triangleIndicies(fromRectangle: n)
      
      }.joined())
  }
  
  private static func lineIndicies(width: Int, height: Int) -> [Index] {
    return Array((0..<(width * height)).map { (n) -> [Index] in
      return IndiciesLayout.lineIndicies(fromRectangle: n)
      
      }.joined())
  }
  
  private static func indicies(width: Int, height: Int, primitiveType: MTLPrimitiveType) -> [Index] {
    switch primitiveType {
    case .triangle: return triangleIndicies(width: width, height: height)
    case .line: return lineIndicies(width: width, height: height)
    default: return []
    }
  }

}


extension GridDescriptor {
  
  var metalIndexType: MTLIndexType { return IndiciesLayout.metalIndexType }
  
  var area: Int { return width * height }
  
  var indicies: [IndiciesLayout.Index] {
    return Self.indicies(width: width, height: height, primitiveType: primitiveType)
  }
  
  var matrix: Matrix { return Self.matrix(width: width, height: height) }
  
  func loadVertexBuffer(_ buffer: MTLBuffer, with blocks: [Block]) {
    let pointerBuffer = Array(verticies(for: blocks)).withUnsafeBytes({ return $0 })
    guard let baseAddress = pointerBuffer.baseAddress else { return }
    
    buffer.contents().copyBytes(from: baseAddress, count: pointerBuffer.count)
  }
  
}


/// Placeholder color structure with presets.
fileprivate enum Color {
  
  case white
  case black
  case red
  case green
  case blue
  case cyan
  case magenta
  case yellow
  
  var value: float4 {
    switch self {
    case .white: return float4(1, 1, 1, 1)
    case .black: return float4(0, 0, 0, 1)
    case .red: return float4(1, 0, 0, 1)
    case .green: return float4(0, 1, 0, 1)
    case .blue: return float4(0, 0, 1, 1)
    case .cyan: return float4(0, 1, 1, 1)
    case .magenta: return float4(1, 0, 1, 1)
    case .yellow: return float4(1, 1, 0, 1)
    }
  }
  
  var hightlight: float4 { return self.value + float4(0.1, 0.1, 0.1, 0.0)}
  var shadow: float4 { return self.value - float4(0.2, 0.2, 0.2, 0.0) }
  
}


/// Graphical data required to fill an entire grid item rectangle.
struct TriangleFillGridDescriptor: GridDescriptor {
  
  struct IndiciesLayout: RectangleCornerLayout {
    
    typealias Index = uint16
    
    static var bottomLeftCornerOffset: Index { return 0 }
    static var bottomRightCornerOffset: Index { return 1 }
    static var topLeftCornerOffset: Index { return 2 }
    static var topRightCornerOffset: Index { return 3 }
    
  }
  
  let width: Int
  let height: Int
  
  var primitiveType: MTLPrimitiveType { return MTLPrimitiveType.triangle }
  
  /// Fixed fill color for formations.
  private static func color(for formation: Formation) -> Color {
    switch formation {
    case .Oh: return .red
    case .Ie: return .white
    case .Sa: return .yellow
    case .Ze: return .green
    case .Lo: return .blue
    case .Je: return .cyan
    case .To: return .magenta
    }
  }
  
  func verticies(for blocks: [Block]) -> AnySequence<Vertex> {
    return AnySequence(blocks.filter({ (block) -> Bool in return !block.isEmpty })
      .map { (block) -> [Vertex] in
        
        let color = TriangleFillGridDescriptor.color(for: block.formation!)
        let (bl, br, tl, tr) = block.position.corners
        
        return [Vertex(position: float2(Float(bl.x), Float(bl.y)), color: color.shadow),
                Vertex(position: float2(Float(br.x), Float(br.y)), color: color.shadow),
                Vertex(position: float2(Float(tl.x), Float(tl.y)), color: color.value),
                Vertex(position: float2(Float(tr.x), Float(tr.y)), color: color.value)]
      }.joined())
  }
  
}


/// Graphical data needed to draw a white line border around a single grid item rectanlge.
struct LineBorderGridDescriptor: GridDescriptor {
  
  struct IndiciesLayout: RectangleCornerLayout {
    
    typealias Index = uint16
    
    static var bottomLeftCornerOffset: Index { return 0 }
    static var bottomRightCornerOffset: Index { return 1 }
    static var topLeftCornerOffset: Index { return 2 }
    static var topRightCornerOffset: Index { return 3 }
    
  }
  
  let width: Int
  let height: Int
  
  var primitiveType: MTLPrimitiveType { return MTLPrimitiveType.line }
  
  func verticies(for blocks: [Block]) -> AnySequence<Vertex> {
    let color = Color.white.value
    
    return AnySequence(blocks.filter { block in return !block.isEmpty }
      .map { (block) -> [Vertex] in
        let (bl, br, tl, tr) = block.position.corners
        
        return [Vertex(position: float2(Float(bl.x), Float(bl.y)), color: color),
                Vertex(position: float2(Float(br.x), Float(br.y)), color: color),
                Vertex(position: float2(Float(tl.x), Float(tl.y)), color: color),
                Vertex(position: float2(Float(tr.x), Float(tr.y)), color: color)]
      }.joined())
  }
  
}


