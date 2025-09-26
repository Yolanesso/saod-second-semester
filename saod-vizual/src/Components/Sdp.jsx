import React, { useState, useEffect, useRef } from "react";
import "../css/Sdp.css";

class TreeNode {
  constructor(value) {
    this.value = value;
    this.left = null;
    this.right = null;
    this.x = 0;
    this.y = 0;
    this.id = Math.random().toString(36).substr(2, 9);
  }
}

class BinarySearchTree {
  constructor() {
    this.root = null;
    this.maxLevel = 0;
  }

  insert(value) {
    const newNode = new TreeNode(value);

    if (!this.root) {
      this.root = newNode;
      return this;
    }

    let current = this.root;
    while (true) {
      if (value === current.value) return undefined;

      if (value < current.value) {
        if (!current.left) {
          current.left = newNode;
          return this;
        }
        current = current.left;
      } else {
        if (!current.right) {
          current.right = newNode;
          return this;
        }
        current = current.right;
      }
    }
  }

  generateRandomTree(nodeCount = 12) {
    this.root = null;
    const values = new Set();

    while (values.size < nodeCount) {
      const randomValue = Math.floor(Math.random() * 100) + 1;
      if (!values.has(randomValue)) {
        values.add(randomValue);
        this.insert(randomValue);
      }
    }

    return this;
  }

  calculateTreeDepth(node) {
    if (!node) return 0;
    return (
      1 +
      Math.max(
        this.calculateTreeDepth(node.left),
        this.calculateTreeDepth(node.right)
      )
    );
  }

  calculatePositions(canvasWidth, canvasHeight) {
    if (!this.root) return;

    const depth = this.calculateTreeDepth(this.root);
    this.maxLevel = depth;

    const horizontalSpacing = canvasWidth / (Math.pow(2, depth) + 1);
    const verticalSpacing = (canvasHeight - 200) / (depth + 1);

    const positionNode = (node, level, position, minX, maxX) => {
      if (!node) return;

      node.x = (minX + maxX) / 2;
      node.y = 100 + level * verticalSpacing;

      const horizontalRange = (maxX - minX) / 2;

      positionNode(
        node.left,
        level + 1,
        position * 2,
        minX,
        minX + horizontalRange
      );
      positionNode(
        node.right,
        level + 1,
        position * 2 + 1,
        minX + horizontalRange,
        maxX
      );
    };

    positionNode(
      this.root,
      0,
      0,
      horizontalSpacing,
      canvasWidth - horizontalSpacing
    );
  }

  getNodesAndEdges() {
    const nodes = [];
    const edges = [];

    const traverse = (node) => {
      if (!node) return;

      nodes.push(node);

      if (node.left) {
        edges.push({
          from: node,
          to: node.left,
          id: `${node.id}-${node.left.id}`,
        });
        traverse(node.left);
      }

      if (node.right) {
        edges.push({
          from: node,
          to: node.right,
          id: `${node.id}-${node.right.id}`,
        });
        traverse(node.right);
      }
    };

    traverse(this.root);
    return { nodes, edges };
  }
}

const BinaryTreeVisualizer = () => {
  const [tree, setTree] = useState(new BinarySearchTree());
  const [nodes, setNodes] = useState([]);
  const [edges, setEdges] = useState([]);
  const [canvasSize, setCanvasSize] = useState({
    width: window.innerWidth,
    height: window.innerHeight - 200,
  });
  const canvasRef = useRef(null);

  const updateCanvasSize = () => {
    setCanvasSize({
      width: window.innerWidth - 40,
      height: window.innerHeight - 200,
    });
  };

  const generateNewTree = () => {
    const newTree = new BinarySearchTree();
    const nodeCount = Math.floor(Math.random() * 8) + 8; // 8-15 узлов
    newTree.generateRandomTree(nodeCount);

    updateCanvasSize();
    newTree.calculatePositions(canvasSize.width, canvasSize.height);

    const { nodes: newNodes, edges: newEdges } = newTree.getNodesAndEdges();

    setTree(newTree);
    setNodes(newNodes);
    setEdges(newEdges);
  };

  useEffect(() => {
    generateNewTree();

    const handleResize = () => {
      updateCanvasSize();
      if (tree.root) {
        tree.calculatePositions(canvasSize.width, canvasSize.height);
        const { nodes: updatedNodes, edges: updatedEdges } =
          tree.getNodesAndEdges();
        setNodes(updatedNodes);
        setEdges(updatedEdges);
      }
    };

    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
  }, []);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Рисуем связи
    ctx.strokeStyle = "#6366f1";
    ctx.lineWidth = 3;
    ctx.lineCap = "round";

    edges.forEach((edge) => {
      ctx.beginPath();
      ctx.moveTo(edge.from.x, edge.from.y);
      ctx.lineTo(edge.to.x, edge.to.y);
      ctx.stroke();
    });

    // Рисуем узлы
    nodes.forEach((node) => {
      // Внешний круг
      ctx.beginPath();
      ctx.arc(node.x, node.y, 20, 0, 2 * Math.PI);
      ctx.fillStyle = "#4f46e5";
      ctx.fill();
      ctx.strokeStyle = "#3730a3";
      ctx.lineWidth = 2;
      ctx.stroke();

      // Внутренний круг
      ctx.beginPath();
      ctx.arc(node.x, node.y, 16, 0, 2 * Math.PI);
      ctx.fillStyle = "#6366f1";
      ctx.fill();

      // Текст
      ctx.font = "bold 14px Inter, sans-serif";
      ctx.fillStyle = "white";
      ctx.textAlign = "center";
      ctx.textBaseline = "middle";
      ctx.fillText(node.value.toString(), node.x, node.y);
    });
  }, [nodes, edges, canvasSize]);

  return (
    <div className="app">
      <div className="header">
        <h1>🌳 Визуализатор Бинарных Деревьев</h1>
        <p>
          Случайные бинарные деревья поиска с автоматическим масштабированием
        </p>
      </div>

      <div className="canvas-container">
        <canvas
          ref={canvasRef}
          width={canvasSize.width}
          height={canvasSize.height}
          className="tree-canvas"
        />
        <div className="canvas-border"></div>
      </div>

      <div className="controls">
        <button onClick={generateNewTree} className="generate-btn">
          🔄 Сгенерировать новое дерево
        </button>

        <div className="info">
          <span>Узлов: {nodes.length}</span>
          <span>Уровней: {tree.maxLevel || 0}</span>
          <span>
            Размер: {canvasSize.width}x{canvasSize.height}px
          </span>
        </div>
      </div>
    </div>
  );
};

export default BinaryTreeVisualizer;
