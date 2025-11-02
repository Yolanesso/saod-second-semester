// components/Tree.js
import React from "react";

const TreeNode = ({ node, x, y, level, maxLevel }) => {
  if (!node) return null;

  const horizontalSpacing = 120;
  const verticalSpacing = 80;
  const levelWidth = Math.pow(2, level) * horizontalSpacing;

  const leftX = x - levelWidth / 4;
  const rightX = x + levelWidth / 4;
  const nextY = y + verticalSpacing;

  return (
    <>
      {/* Линии к детям */}
      {node.left && (
        <line
          x1={x}
          y1={y + 20}
          x2={leftX}
          y2={nextY - 20}
          stroke="#4A90E2"
          strokeWidth="2"
        />
      )}
      {node.right && (
        <line
          x1={x}
          y1={y + 20}
          x2={rightX}
          y2={nextY - 20}
          stroke="#4A90E2"
          strokeWidth="2"
        />
      )}

      {/* Узел */}
      <g>
        <circle
          cx={x}
          cy={y}
          r="25"
          fill="#fff"
          stroke="#4A90E2"
          strokeWidth="3"
        />
        <text
          x={x}
          y={y - 5}
          textAnchor="middle"
          fontSize="14"
          fontWeight="bold"
          fill="#2C3E50"
        >
          {node.key}
        </text>
        <text x={x} y={y + 15} textAnchor="middle" fontSize="10" fill="#7F8C8D">
          w:{node.weight}
        </text>
      </g>

      {/* Рекурсивно рендерим детей */}
      <TreeNode
        node={node.left}
        x={leftX}
        y={nextY}
        level={level + 1}
        maxLevel={maxLevel}
      />
      <TreeNode
        node={node.right}
        x={rightX}
        y={nextY}
        level={level + 1}
        maxLevel={maxLevel}
      />
    </>
  );
};

const Tree = ({ data }) => {
  const calculateTreeHeight = (node) => {
    if (!node) return 0;
    return (
      1 +
      Math.max(calculateTreeHeight(node.left), calculateTreeHeight(node.right))
    );
  };

  const height = calculateTreeHeight(data);
  const svgWidth = Math.pow(2, height) * 60;
  const svgHeight = height * 100 + 100;

  return (
    <div className="tree-container">
      <svg
        width={svgWidth}
        height={svgHeight}
        viewBox={`0 0 ${svgWidth} ${svgHeight}`}
      >
        <TreeNode
          node={data}
          x={svgWidth / 2}
          y={60}
          level={0}
          maxLevel={height}
        />
      </svg>
    </div>
  );
};

export default Tree;
