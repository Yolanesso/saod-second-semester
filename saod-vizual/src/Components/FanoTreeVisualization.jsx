import React, { useMemo } from 'react';
import '../css/FanoTree.css';

function FanoTreeVisualization({ treeData }) {
  const layout = useMemo(() => {
    if (!treeData || !treeData.root) return null;
    return calculateLayout(treeData.root);
  }, [treeData]);

  if (!treeData || !treeData.root || !layout) {
    return (
      <div className="tree-empty">
        <p>Введите текст для построения дерева Фано</p>
      </div>
    );
  }

  const { width, height, nodes } = layout;
  const padding = 50;
  const svgWidth = width + padding * 2;
  const svgHeight = height + padding * 2;

  return (
    <div className="fano-tree-container">
      <div className="tree-svg-wrapper">
        <svg
          width={svgWidth}
          height={svgHeight}
          viewBox={`0 0 ${svgWidth} ${svgHeight}`}
          className="tree-svg"
        >
          {/* Рисуем связи с метками 0/1 - строим из структуры дерева */}
          {(() => {
            const edges = [];
            const nodeByOriginal = new Map();
            nodes.forEach(node => {
              if (node.originalNode) {
                nodeByOriginal.set(node.originalNode, node);
              }
            });

            // Проходим по всем узлам и строим связи из структуры дерева
            nodes.forEach(node => {
              if (!node.originalNode || node.isLeaf) return;
              
              const originalNode = node.originalNode;
              
              // Левая связь (0)
              if (originalNode.left) {
                const leftChild = nodeByOriginal.get(originalNode.left);
                if (leftChild) {
                  const midX = (node.x + leftChild.x) / 2 + padding;
                  const midY = (node.y + leftChild.y) / 2 + padding;
                  edges.push({
                    key: `edge-${node.id}-left`,
                    x1: node.x + padding,
                    y1: node.y + padding,
                    x2: leftChild.x + padding,
                    y2: leftChild.y + padding,
                    midX,
                    midY,
                    label: '0'
                  });
                }
              }
              
              // Правая связь (1)
              if (originalNode.right) {
                const rightChild = nodeByOriginal.get(originalNode.right);
                if (rightChild) {
                  const midX = (node.x + rightChild.x) / 2 + padding;
                  const midY = (node.y + rightChild.y) / 2 + padding;
                  edges.push({
                    key: `edge-${node.id}-right`,
                    x1: node.x + padding,
                    y1: node.y + padding,
                    x2: rightChild.x + padding,
                    y2: rightChild.y + padding,
                    midX,
                    midY,
                    label: '1'
                  });
                }
              }
            });

            return edges.map(edge => (
              <g key={edge.key}>
                <line
                  x1={edge.x1}
                  y1={edge.y1}
                  x2={edge.x2}
                  y2={edge.y2}
                  className="tree-line"
                  strokeWidth="2.5"
                />
                {/* Метка 0 или 1 на связи */}
                <circle
                  cx={edge.midX}
                  cy={edge.midY}
                  r="12"
                  fill="white"
                  stroke="#667eea"
                  strokeWidth="2"
                />
                <text
                  x={edge.midX}
                  y={edge.midY + 4}
                  textAnchor="middle"
                  className="edge-label"
                  fontSize="11"
                  fontWeight="700"
                  fill="#667eea"
                >
                  {edge.label}
                </text>
              </g>
            ));
          })()}

          {/* Рисуем только листья (внутренние узлы скрыты) */}
          {nodes.map((node) => {
            // Пропускаем внутренние узлы - не отображаем их
            if (!node.isLeaf) return null;
            
            return (
              <g key={node.id}>
                {/* Круг узла */}
                <circle
                  cx={node.x + padding}
                  cy={node.y + padding}
                  r={node.radius}
                  className="tree-node leaf-node"
                />
                
                {/* Символ для листьев */}
                <text
                  x={node.x + padding}
                  y={node.y + padding - 8}
                  textAnchor="middle"
                  className="node-label"
                  fontSize="14"
                  fontWeight="600"
                >
                  {node.displayLabel}
                </text>
                {/* Код для листьев */}
                {node.code && (
                  <text
                    x={node.x + padding}
                    y={node.y + padding + 18}
                    textAnchor="middle"
                    className="node-code"
                    fontSize="11"
                    fontWeight="600"
                  >
                    {node.code}
                  </text>
                )}
              </g>
            );
          })}
        </svg>
      </div>
    </div>
  );
}

function calculateLayout(root) {
  const nodes = [];
  const nodeMap = new WeakMap(); // Маппинг оригинального узла -> визуальный узел
  const minHorizontalSpacing = 180; // Расстояние между поддеревьями
  const verticalSpacing = 140; // Расстояние по вертикали
  let nodeId = 0;
  let currentX = 0; // Текущая позиция X для размещения листьев

  // Рекурсивно размещаем узлы (обход в глубину)
  function assignPositions(node, y, parentId = null) {
    if (!node) return null;
    
    // Проверяем, не обрабатывали ли мы уже этот узел
    if (nodeMap.has(node)) {
      // Если узел уже обработан, возвращаем его позицию
      const existingNode = nodeMap.get(node);
      return { x: existingNode.x, y: existingNode.y };
    }

    const id = nodeId++;
    const isLeaf = node.isLeaf();

    let x;
    if (isLeaf) {
      // Листья размещаем последовательно слева направо
      x = currentX;
      currentX += minHorizontalSpacing;
    } else {
      // Внутренние узлы размещаем между детьми
      // В двоичном дереве каждый внутренний узел должен иметь ровно двух детей
      if (!node.left || !node.right) {
        console.warn('Внутренний узел должен иметь двух детей!', node);
        return null;
      }
      
      const leftPos = assignPositions(node.left, y + verticalSpacing, id);
      const rightPos = assignPositions(node.right, y + verticalSpacing, id);
      
      // Оба ребенка должны существовать для двоичного дерева
      if (leftPos && rightPos) {
        x = (leftPos.x + rightPos.x) / 2;
      } else {
        console.warn('Не удалось разместить детей узла', node);
        x = 0;
      }
    }

    let displayLabel = '';
    if (isLeaf) {
      if (node.char === ' ') {
        displayLabel = '␣';
      } else if (node.char === '\n') {
        displayLabel = '\\n';
      } else if (node.char === '\t') {
        displayLabel = '\\t';
      } else {
        displayLabel = node.char;
      }
    } else {
      displayLabel = node.frequency.toString();
    }

    const nodeData = {
      id,
      x,
      y,
      char: node.char,
      frequency: node.frequency,
      code: node.code,
      isLeaf,
      displayLabel,
      radius: isLeaf ? 35 : 30,
      parentId,
      originalNode: node, // Сохраняем ссылку на оригинальный узел
    };

    nodes.push(nodeData);
    nodeMap.set(node, nodeData); // Сохраняем маппинг

    return { x, y };
  }

  // Начинаем размещение с корня
  assignPositions(root, 0);

  // Фильтруем только уникальные узлы по оригинальному узлу
  // В дереве Хаффмана каждый узел должен быть уникальным
  const uniqueNodes = [];
  const seenOriginalNodes = new WeakSet();
  
  for (const node of nodes) {
    if (node.originalNode && seenOriginalNodes.has(node.originalNode)) {
      continue; // Пропускаем дубликаты
    }
    if (node.originalNode) {
      seenOriginalNodes.add(node.originalNode);
    }
    uniqueNodes.push(node);
  }

  // Вычисляем границы и нормализуем координаты
  if (uniqueNodes.length === 0) {
    return { width: 0, height: 0, nodes: [] };
  }

  const minX = Math.min(...uniqueNodes.map((n) => n.x - n.radius));
  const maxX = Math.max(...uniqueNodes.map((n) => n.x + n.radius));
  const minY = Math.min(...uniqueNodes.map((n) => n.y - n.radius));
  const maxY = Math.max(...uniqueNodes.map((n) => n.y + n.radius + 50)); // +50 для текста под узлом

  // Нормализуем координаты
  uniqueNodes.forEach((node) => {
    node.x = node.x - minX + 60;
    node.y = node.y - minY + 60;
  });

  return {
    width: maxX - minX + 120,
    height: maxY - minY + 120,
    nodes: uniqueNodes,
  };
}

export default FanoTreeVisualization;

