//
//  ViewController.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/8/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import Cocoa


class BlockItemView: NSCollectionViewItem {
  
  class View: NSView {
    
    override func draw(_ dirtyRect: NSRect) {
      NSColor.red.setFill()
      NSBezierPath(rect: dirtyRect).fill()
    }
    
  }
  
  override func viewDidLoad() {
    super.viewDidLoad()
    
    view.addSubview(View(frame: view.frame))
    
    view.wantsLayer = true
  }
}


class ShapeCollectionView: NSCollectionView {
  
  required init?(coder decoder: NSCoder) {
    super.init(coder: decoder)
    
    let layout = NSCollectionViewGridLayout()
    layout.maximumNumberOfRows = Shape.rowAmount
    layout.maximumNumberOfColumns = Shape.columnAmount
    layout.minimumInteritemSpacing = 3.0
    layout.minimumLineSpacing = 3.0
    
    collectionViewLayout = layout
  }
  
}


class ViewController: NSViewController {
  
  @IBOutlet var nextShapeView: ShapeCollectionView?
  
  @IBAction func breakPoint(_ sender: Any?) { }
  
}


extension ViewController: NSCollectionViewDataSource {
  
  func numberOfSections(in collectionView: NSCollectionView) -> Int { return 1 }
  
  var shape: Shape? {
    let state = js_alloc_tetris_state()
    js_init_tetris_state(nil)
    
    guard let shape = state?.pointee.shape.pointee else { return nil }
    
    js_dealloc_tetris_state(state)
    
    return Shape(shape)
  }
  
  func collectionView(_ collectionView: NSCollectionView, numberOfItemsInSection section: Int) -> Int {
    guard section == 0 else { return 0 }
    return Shape.rowAmount * Shape.columnAmount
  }
  
  func collectionView(_ collectionView: NSCollectionView, itemForRepresentedObjectAt indexPath: IndexPath) -> NSCollectionViewItem {
    print("shape: \(shape!)")
    
    let item = collectionView.makeItem(withIdentifier: NSUserInterfaceItemIdentifier("BlockItemView"), for: indexPath)
    
    return item
  }
  
}
