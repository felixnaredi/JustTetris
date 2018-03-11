//
//  ViewController.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/8/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import Cocoa
import simd
import MetalKit


class RedView: NSView {
  
  override func draw(_ dirtyRect: NSRect) {
    NSColor.red.setFill()
    NSBezierPath(rect: dirtyRect).fill()
  }
  
}


class ShapeView: MTKView {
  
  var mouseDownHandler: ((NSEvent) -> Void)?
  
  override func mouseDown(with event: NSEvent) {
    super.mouseDown(with: event)
    
    mouseDownHandler?(event)
  }
  
  override var clearColor: MTLClearColor {
    get { return MTLClearColor(red: 0, green: 0, blue: 0, alpha: 0) }
    set { }
  }
  
}


class ViewController: NSViewController {
  
  @IBOutlet var shapeView: ShapeView!
  
  var shapeViewRenderer: ShapeRenderer? {
    didSet { shapeView.delegate = shapeViewRenderer }
  }
  
  var shape: Shape? {
    didSet {
      guard let blocks = shape?.blocks else { return }
      shapeViewRenderer?.blocks = blocks
    }
  }
  
  @IBAction func breakPoint(_ sender: Any?) { }
  
  static func makeShape() -> Shape? {
    guard let state = js_alloc_tetris_state() else { return nil }
    js_init_tetris_state(state)
    
    let shape = Shape(state.pointee.shape.pointee)
    
    js_dealloc_tetris_state(state)
    
    return shape
  }
  
  func changeShape() {
    shape = ViewController.makeShape()
    print(shape)
    shapeView.needsDisplay = true
  }
  
  override func viewDidLoad() {
    guard let shape = ViewController.makeShape() else {
      print("failed to make shape")
      return
    }
    
    print(shape)
    
    guard let renderContext = RenderContext(with: shapeView) else { return }
    guard let gridDescriptor = ShapeGridDescriptor(with: renderContext.device) else { return }
    
    shapeViewRenderer = ShapeRenderer(renderContext: renderContext, gridDescriptor: gridDescriptor)
    shapeViewRenderer?.blocks = shape.blocks
    
    shapeView.mouseDownHandler = { _ in self.changeShape() }
  }
  
}
