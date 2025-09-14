import React, { useState, useEffect, useRef } from "react";
import "../css/BinaryTree.css";

const TreeNode = ({ node, x, y, level, isHighlighted }) => {
  const [isPulsing, setIsPulsing] = useState(false);

  useEffect(() => {
    if (isHighlighted) {
      setIsPulsing(true);
      const timer = setTimeout(() => setIsPulsing(false), 600);
      return () => clearTimeout(timer);
    }
  }, [isHighlighted]);

  return (
    <g className={`tree-node ${isPulsing ? "pulse" : ""}`}>
      <circle
        cx={x}
        cy={y}
        r="20"
        className={`node-circle ${isHighlighted ? "highlighted" : ""}`}
      />
      <text x={x} y={y} textAnchor="middle" dy=".3em" className="node-value">
        {node.value}
      </text>
    </g>
  );
};

const TreeEdge = ({ fromX, fromY, toX, toY }) => {
  return <line x1={fromX} y1={fromY} x2={toX} y2={toY} className="tree-edge" />;
};

const BinaryTreeVisualizer = () => {
  const [tree, setTree] = useState(null);
  const [inputValue, setInputValue] = useState("");
  const [highlightedNode, setHighlightedNode] = useState(null);
  const [svgDimensions, setSvgDimensions] = useState({
    width: 800,
    height: 600,
  });
  const svgRef = useRef();
  const containerRef = useRef();

  class TreeNodeClass {
    constructor(value) {
      this.value = value;
      this.left = null;
      this.right = null;
    }
  }

  // Функция для обновления размеров SVG
  const updateSvgDimensions = () => {
    if (containerRef.current) {
      const width = containerRef.current.clientWidth - 40; // Отступы
      const height = containerRef.current.clientHeight - 40;
      setSvgDimensions({ width, height });
    }
  };

  useEffect(() => {
    updateSvgDimensions();
    window.addEventListener("resize", updateSvgDimensions);

    return () => {
      window.removeEventListener("resize", updateSvgDimensions);
    };
  }, []);

  const insertNode = (root, value) => {
    if (!root) return new TreeNodeClass(value);

    if (value < root.value) {
      root.left = insertNode(root.left, value);
    } else {
      root.right = insertNode(root.right, value);
    }

    return root;
  };

  const buildBalancedTree = (sortedArray, start, end) => {
    if (start > end) return null;

    const mid = Math.floor((start + end) / 2);
    const node = new TreeNodeClass(sortedArray[mid]);

    node.left = buildBalancedTree(sortedArray, start, mid - 1);
    node.right = buildBalancedTree(sortedArray, mid + 1, end);

    return node;
  };

  const addNode = () => {
    if (!inputValue.trim()) return;

    const value = parseInt(inputValue);
    if (isNaN(value)) return;

    const values = tree ? treeToArray(tree) : [];
    values.push(value);
    values.sort((a, b) => a - b);

    const newTree = buildBalancedTree(values, 0, values.length - 1);
    setTree(newTree);
    setInputValue("");
    setHighlightedNode(value);
  };

  const treeToArray = (node, array = []) => {
    if (!node) return array;
    treeToArray(node.left, array);
    array.push(node.value);
    treeToArray(node.right, array);
    return array;
  };

  const clearTree = () => {
    setTree(null);
    setHighlightedNode(null);
  };

  const generateRandomTree = () => {
    const size = Math.floor(Math.random() * 15) + 5;
    const values = Array.from(
      { length: size },
      () => Math.floor(Math.random() * 100) + 1
    );
    values.sort((a, b) => a - b);

    const newTree = buildBalancedTree(values, 0, values.length - 1);
    setTree(newTree);
    setHighlightedNode(null);
  };

  const renderTree = (node, x, y, level, dx, nodes = [], edges = []) => {
    if (!node) return { nodes, edges };

    const nodeElement = (
      <TreeNode
        key={`node-${node.value}-${x}-${y}`}
        node={node}
        x={x}
        y={y}
        level={level}
        isHighlighted={highlightedNode === node.value}
      />
    );
    nodes.push(nodeElement);

    if (node.left) {
      const leftX = x - dx;
      const leftY = y + 80;

      edges.push(
        <TreeEdge
          key={`edge-${node.value}-left`}
          fromX={x}
          fromY={y + 20}
          toX={leftX}
          toY={leftY - 20}
        />
      );

      renderTree(node.left, leftX, leftY, level + 1, dx / 2, nodes, edges);
    }

    if (node.right) {
      const rightX = x + dx;
      const rightY = y + 80;

      edges.push(
        <TreeEdge
          key={`edge-${node.value}-right`}
          fromX={x}
          fromY={y + 20}
          toX={rightX}
          toY={rightY - 20}
        />
      );

      renderTree(node.right, rightX, rightY, level + 1, dx / 2, nodes, edges);
    }

    return { nodes, edges };
  };

  const getTreeHeight = (node) => {
    if (!node) return 0;
    return 1 + Math.max(getTreeHeight(node.left), getTreeHeight(node.right));
  };

  const calculateTreeLayout = () => {
    if (!tree) return { nodes: [], edges: [] };

    const height = getTreeHeight(tree);
    const dx = Math.min(svgDimensions.width / (Math.pow(2, height) + 1), 200);

    return renderTree(tree, svgDimensions.width / 2, 60, 0, dx);
  };

  const treeData = calculateTreeLayout();

  return (
    <div className="binary-tree-container">
      <div className="controls-panel">
        <h2>🌳 Визуализатор ИСДП</h2>

        <div className="input-group">
          <input
            type="number"
            value={inputValue}
            onChange={(e) => setInputValue(e.target.value)}
            placeholder="Введите число..."
            className="tree-input"
            onKeyPress={(e) => e.key === "Enter" && addNode()}
          />
          <button onClick={addNode} className="btn btn-primary">
            Добавить узел
          </button>
        </div>

        <div className="button-group">
          <button onClick={generateRandomTree} className="btn btn-secondary">
            Случайное дерево
          </button>
          <button onClick={clearTree} className="btn btn-danger">
            Очистить
          </button>
        </div>

        {tree && (
          <div className="tree-info">
            <p>Узлов: {treeToArray(tree).length}</p>
            <p>Высота: {getTreeHeight(tree)}</p>
          </div>
        )}
      </div>

      <div className="tree-visualization" ref={containerRef}>
        <svg
          ref={svgRef}
          width={svgDimensions.width}
          height={svgDimensions.height}
          className="tree-svg"
          viewBox={`0 0 ${svgDimensions.width} ${svgDimensions.height}`}
          preserveAspectRatio="xMidYMid meet"
        >
          {treeData.edges}
          {treeData.nodes}
        </svg>

        {!tree && (
          <div className="empty-state">
            <div className="empty-icon">🌲</div>
            <p>Дерево пустое</p>
            <span>Добавьте узлы или создайте случайное дерево</span>
          </div>
        )}
      </div>
    </div>
  );
};

export default BinaryTreeVisualizer;
