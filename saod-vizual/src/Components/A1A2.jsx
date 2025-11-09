import React, { useState, useEffect, useRef } from "react";
import "../css/A1A2.css";

const App = () => {
  const [nodeCount, setNodeCount] = useState(10);
  const [a1Tree, setA1Tree] = useState(null);
  const [a2Tree, setA2Tree] = useState(null);
  const [stepByStep, setStepByStep] = useState([]);
  const [currentStep, setCurrentStep] = useState(0);
  const [algorithm, setAlgorithm] = useState("A2");
  const [isAnimating, setIsAnimating] = useState(false);
  const [animatedNodes, setAnimatedNodes] = useState(new Set());
  const [highlightedNode, setHighlightedNode] = useState(null);
  const [weights, setWeights] = useState({});
  const animationIntervalRef = useRef(null);

  class TreeNode {
    constructor(key, weight) {
      this.key = key;
      this.weight = weight;
      this.left = null;
      this.right = null;
      this.x = 0;
      this.y = 0;
      this.id = Math.random();
    }
  }

  // Генерация чисел со случайными весами
  const generateNumbersWithWeights = (count) => {
    const numbers = [];
    const weightsObj = {};
    
    // Генерируем уникальные числа от 1 до count * 2 для разнообразия
    const maxValue = Math.max(count * 2, 20);
    const availableNumbers = Array.from({ length: maxValue }, (_, i) => i + 1);
    
    // Перемешиваем и берем первые count чисел
    for (let i = availableNumbers.length - 1; i > 0; i--) {
      const j = Math.floor(Math.random() * (i + 1));
      [availableNumbers[i], availableNumbers[j]] = [availableNumbers[j], availableNumbers[i]];
    }
    
    const selectedNumbers = availableNumbers.slice(0, count).sort((a, b) => a - b);
    
    selectedNumbers.forEach(num => {
      // Генерируем случайный вес от 1 до 50
      const weight = Math.floor(Math.random() * 50) + 1;
      weightsObj[num] = weight;
      numbers.push([num, weight]);
    });
    
    setWeights(weightsObj);
    return weightsObj;
  };

  // Алгоритм A1
  const buildA1Tree = (weights) => {
    const entries = Object.entries(weights).map(([k, v]) => [Number(k), v]);
    // Сортируем по убыванию весов
    entries.sort((a, b) => b[1] - a[1]);

    const steps = [];
    let root = null;

    entries.forEach(([key, weight], index) => {
      const newNode = new TreeNode(key, weight);
      steps.push({
        action: "start_insert",
        key,
        weight,
        tree: deepCloneTree(root),
        message: `Шаг ${index + 1}: Начинаем вставку числа ${key} с весом ${weight}`,
        insertedKey: key,
      });

      if (!root) {
        root = newNode;
        steps.push({
          action: "set_root",
          key,
          tree: deepCloneTree(root),
          message: `Число ${key} становится корнем дерева`,
          insertedKey: key,
        });
      } else {
        let current = root;
        let parent = null;
        const path = [];

        while (current) {
          parent = current;
          path.push(current.key);
          if (key < current.key) {
            current = current.left;
            steps.push({
              action: "go_left",
              key,
              current: current?.key,
              parent: parent.key,
              tree: deepCloneTree(root),
              message: `${key} < ${parent.key} → идем влево`,
              highlightedKey: parent.key,
            });
          } else {
            current = current.right;
            steps.push({
              action: "go_right",
              key,
              current: current?.key,
              parent: parent.key,
              tree: deepCloneTree(root),
              message: `${key} ≥ ${parent.key} → идем вправо`,
              highlightedKey: parent.key,
            });
          }
        }

        if (key < parent.key) {
          parent.left = newNode;
          steps.push({
            action: "insert_left",
            key,
            parent: parent.key,
            tree: deepCloneTree(root),
            message: `Вставляем ${key} как левого потомка ${parent.key}`,
            insertedKey: key,
            highlightedKey: parent.key,
          });
        } else {
          parent.right = newNode;
          steps.push({
            action: "insert_right",
            key,
            parent: parent.key,
            tree: deepCloneTree(root),
            message: `Вставляем ${key} как правого потомка ${parent.key}`,
            insertedKey: key,
            highlightedKey: parent.key,
          });
        }
      }
    });

    return { root, steps };
  };

  // Глубокая копия дерева
  const deepCloneTree = (node) => {
    if (!node) return null;
    const cloned = new TreeNode(node.key, node.weight);
    cloned.left = deepCloneTree(node.left);
    cloned.right = deepCloneTree(node.right);
    return cloned;
  };

  // Алгоритм A2
  const buildA2Tree = (weights) => {
    const entries = Object.entries(weights)
      .map(([k, v]) => [Number(k), v])
      .sort((a, b) => a[0] - b[0]); // Сортируем по числам

    const steps = [];
    let stepCounter = 0;

    const build = (start, end, depth = 0, parentTree = null) => {
      if (start > end) return null;

      stepCounter++;
      const slice = entries.slice(start, end + 1);
      const totalWeight = slice.reduce((sum, [_, weight]) => sum + weight, 0);
      const halfWeight = totalWeight / 2;

      steps.push({
        action: "build_subtree",
        start,
        end,
        slice: slice.map(([k]) => k),
        totalWeight,
        halfWeight,
        depth,
        step: stepCounter,
        tree: parentTree ? deepCloneTree(parentTree) : null,
        message: `[Глубина ${depth}] Строим поддерево для чисел [${slice
          .map(([k]) => k)
          .join(", ")}]\nСумма весов: ${totalWeight}, половинная сумма: ${halfWeight.toFixed(2)}`,
        checkingKeys: slice.map(([k]) => k),
      });

      let sum = 0;
      let rootIndex = start;
      let currentTree = parentTree ? deepCloneTree(parentTree) : null;

      for (let i = start; i <= end; i++) {
        const [key, weight] = entries[i];
        const prevSum = sum;
        sum += weight;

        steps.push({
          action: "check_root",
          key,
          weight,
          prevSum,
          newSum: sum,
          halfWeight,
          isRoot: prevSum < halfWeight && sum > halfWeight,
          step: stepCounter,
          tree: currentTree,
          message: `Проверяем число ${key} (вес: ${weight})\nСумма слева: ${prevSum.toFixed(2)}, после добавления: ${sum.toFixed(2)}\nПоловинная сумма: ${halfWeight.toFixed(2)}`,
          highlightedKey: key,
          checkingKeys: slice.slice(0, i - start + 1).map(([k]) => k),
        });

        if (prevSum < halfWeight && sum > halfWeight) {
          rootIndex = i;
          steps.push({
            action: "found_root",
            key,
            step: stepCounter,
            tree: currentTree,
            message: `✅ Найден корень: ${key}!\nУсловие: ${prevSum.toFixed(2)} < ${halfWeight.toFixed(2)} < ${sum.toFixed(2)}`,
            insertedKey: key,
          });
          break;
        }
      }

      const [rootKey, rootWeight] = entries[rootIndex];
      const root = new TreeNode(rootKey, rootWeight);

      // Обновляем дерево с новым узлом
      if (!currentTree) {
        currentTree = root;
      } else {
        // Вставляем в дерево (для визуализации)
        insertIntoTree(currentTree, rootKey, rootWeight);
      }

      steps.push({
        action: "create_root",
        key: rootKey,
        weight: rootWeight,
        step: stepCounter,
        tree: deepCloneTree(currentTree),
        message: `Создаем узел ${rootKey} с весом ${rootWeight} как корень поддерева`,
        insertedKey: rootKey,
      });

      const leftTree = build(start, rootIndex - 1, depth + 1, currentTree);
      const rightTree = build(rootIndex + 1, end, depth + 1, currentTree);

      root.left = leftTree;
      root.right = rightTree;

      // Обновляем дерево с детьми
      if (currentTree) {
        updateTreeWithChildren(currentTree, rootKey, leftTree, rightTree);
      }

      if (root.left || root.right) {
        steps.push({
          action: "build_children",
          key: rootKey,
          left: root.left?.key,
          right: root.right?.key,
          step: stepCounter,
          tree: deepCloneTree(currentTree),
          message: `Построили потомков для ${rootKey}:\nСлева: ${root.left?.key || "нет"}, Справа: ${root.right?.key || "нет"}`,
          insertedKey: rootKey,
        });
      }

      return root;
    };

    const root = build(0, entries.length - 1);
    return { root, steps };
  };

  // Вспомогательные функции для работы с деревом
  const insertIntoTree = (tree, key, weight) => {
    if (!tree) return new TreeNode(key, weight);
    if (key < tree.key) {
      tree.left = insertIntoTree(tree.left, key, weight);
    } else if (key > tree.key) {
      tree.right = insertIntoTree(tree.right, key, weight);
    }
    return tree;
  };

  const updateTreeWithChildren = (tree, key, left, right) => {
    if (!tree) return;
    if (tree.key === key) {
      tree.left = left;
      tree.right = right;
      return;
    }
    if (key < tree.key) {
      updateTreeWithChildren(tree.left, key, left, right);
    } else {
      updateTreeWithChildren(tree.right, key, left, right);
    }
  };

  const buildTree = () => {
    // Останавливаем предыдущую анимацию
    if (animationIntervalRef.current) {
      clearInterval(animationIntervalRef.current);
      animationIntervalRef.current = null;
    }

    const weightsData = generateNumbersWithWeights(nodeCount);

    if (algorithm === "A1") {
      const { root, steps } = buildA1Tree(weightsData);
      setA1Tree(root);
      setA2Tree(null);
      setStepByStep(steps);
    } else {
      const { root, steps } = buildA2Tree(weightsData);
      setA2Tree(root);
      setA1Tree(null);
      setStepByStep(steps);
    }
    setCurrentStep(0);
    setAnimatedNodes(new Set());
    setHighlightedNode(null);
  };

  // Автоматическая анимация построения
  const startAnimation = () => {
    if (stepByStep.length === 0) return;

    setIsAnimating(true);
    setCurrentStep(0);
    setAnimatedNodes(new Set());
    setHighlightedNode(null);

    if (animationIntervalRef.current) {
      clearInterval(animationIntervalRef.current);
    }

    let step = 0;
    animationIntervalRef.current = setInterval(() => {
      if (step < stepByStep.length) {
        setCurrentStep(step);
        const currentStepData = stepByStep[step];
        
        // Добавляем узлы в анимированные
        if (currentStepData.insertedKey) {
          setAnimatedNodes(prev => new Set([...prev, currentStepData.insertedKey]));
        }
        
        // Подсвечиваем текущий узел
        if (currentStepData.highlightedKey) {
          setHighlightedNode(currentStepData.highlightedKey);
        } else {
          setHighlightedNode(null);
        }

        step++;
      } else {
        setIsAnimating(false);
        if (animationIntervalRef.current) {
          clearInterval(animationIntervalRef.current);
          animationIntervalRef.current = null;
        }
      }
    }, 800); // Скорость анимации: 800ms на шаг
  };

  const stopAnimation = () => {
    setIsAnimating(false);
    if (animationIntervalRef.current) {
      clearInterval(animationIntervalRef.current);
      animationIntervalRef.current = null;
    }
  };

  // Автоматическое построение при изменении параметров
  useEffect(() => {
    if (nodeCount > 0) {
      buildTree();
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [nodeCount, algorithm]);

  // Очистка при размонтировании
  useEffect(() => {
    return () => {
      if (animationIntervalRef.current) {
        clearInterval(animationIntervalRef.current);
      }
    };
  }, []);

  // Визуализация дерева
  const TreeVisualization = ({ tree, animatedNodes: animNodes, highlightedNode: highlightNode, isAnimating: animating, algorithm: algo }) => {
    // Вычисление высоты дерева
    const getTreeHeight = (node) => {
      if (!node) return 0;
      return 1 + Math.max(getTreeHeight(node.left), getTreeHeight(node.right));
    };

    // Вычисление количества узлов в поддереве
    const getNodeCount = (node) => {
      if (!node) return 0;
      return 1 + getNodeCount(node.left) + getNodeCount(node.right);
    };

    // Улучшенный алгоритм размещения узлов (Reingold-Tilford стиль)
    const calculatePositions = (node, x, y, level, offset) => {
      if (!node) return x;

      const horizontalSpacing = 180;
      const verticalSpacing = 140; // Увеличенный отступ для узла + блока веса
      
      // Рекурсивно позиционируем детей
      let currentX = x;
      
      if (node.left) {
        currentX = calculatePositions(
          node.left,
          currentX,
          y + verticalSpacing,
          level + 1,
          offset * 0.6
        );
        currentX += horizontalSpacing;
      }
      
      // Позиция текущего узла (y - это верхняя координата области узла)
      node.x = currentX;
      node.y = y;
      
      if (node.right) {
        currentX = calculatePositions(
          node.right,
          currentX + horizontalSpacing,
          y + verticalSpacing,
          level + 1,
          offset * 0.6
        );
      }
      
      return currentX;
    };

    // Центрирование дерева
    const centerTree = (node, offsetX) => {
      if (!node) return;
      
      node.x += offsetX;
      centerTree(node.left, offsetX);
      centerTree(node.right, offsetX);
    };

    const renderNode = (node) => {
      if (!node) return null;

      const nodeRadius = 32;
      const weightBgHeight = 24;
      const nodeCenterY = node.y + nodeRadius;
      const weightBoxY = nodeCenterY + nodeRadius + 8;
      const bottomY = weightBoxY + weightBgHeight;

      // Проверяем, анимирован ли узел
      const isAnimated = animNodes ? animNodes.has(node.key) : false;
      const isHighlighted = highlightNode === node.key;
      const shouldShow = isAnimated || !animating;

      // Координаты для соединения с детьми
      const getChildConnectionY = (childNode) => {
        if (!childNode) return 0;
        const childNodeRadius = 32;
        return childNode.y + childNodeRadius;
      };

      // Определяем цвет узла
      let nodeColor = "url(#nodeGradient)";
      let strokeColor = "#2B6CB0";
      if (isHighlighted) {
        nodeColor = "#F59E0B";
        strokeColor = "#D97706";
      } else if (isAnimated) {
        nodeColor = "#10B981";
        strokeColor = "#059669";
      }

      return (
        <g 
          key={node.id || `${node.key}-${node.x}-${node.y}`}
          className={shouldShow ? "node-visible" : "node-hidden"}
          style={{
            opacity: shouldShow ? 1 : 0,
            transition: "opacity 0.5s ease-in-out"
          }}
        >
          {/* Линии к потомкам (рисуем первыми, чтобы были под узлами) */}
          {node.left && shouldShow && (
            <line
              x1={node.x}
              y1={bottomY}
              x2={node.left.x}
              y2={getChildConnectionY(node.left)}
              stroke="#4A5568"
              strokeWidth="2.5"
              className="tree-edge"
              style={{
                opacity: (animNodes && animNodes.has(node.left.key)) || !animating ? 1 : 0,
                transition: "opacity 0.5s ease-in-out"
              }}
            />
          )}
          {node.right && shouldShow && (
            <line
              x1={node.x}
              y1={bottomY}
              x2={node.right.x}
              y2={getChildConnectionY(node.right)}
              stroke="#4A5568"
              strokeWidth="2.5"
              className="tree-edge"
              style={{
                opacity: (animNodes && animNodes.has(node.right.key)) || !animating ? 1 : 0,
                transition: "opacity 0.5s ease-in-out"
              }}
            />
          )}

          {/* Фон для веса (прямоугольник под узлом) */}
          <rect
            x={node.x - nodeRadius - 8}
            y={weightBoxY}
            width={(nodeRadius + 8) * 2}
            height={weightBgHeight}
            fill={isHighlighted ? "#FEF3C7" : "#E2E8F0"}
            stroke={isHighlighted ? "#F59E0B" : "#2D3748"}
            strokeWidth={isHighlighted ? "3" : "2"}
            rx="6"
            className={`weight-box ${isHighlighted ? "weight-box-highlighted" : ""}`}
          />

          {/* Узел (круг) с градиентом или цветом */}
          <circle
            cx={node.x}
            cy={nodeCenterY}
            r={nodeRadius}
            fill={nodeColor}
            stroke={strokeColor}
            strokeWidth={isHighlighted ? "4" : "3"}
            className={`tree-node-circle ${isHighlighted ? "node-highlighted" : ""} ${isAnimated ? "node-animated" : ""}`}
            style={{
              transform: isHighlighted ? "scale(1.1)" : "scale(1)",
              transition: "all 0.3s ease-in-out",
              filter: isHighlighted ? "drop-shadow(0 0 10px rgba(245, 158, 11, 0.6))" : "drop-shadow(0 2px 4px rgba(0, 0, 0, 0.2))"
            }}
          />

          {/* Ключ узла */}
          <text
            x={node.x}
            y={nodeCenterY}
            textAnchor="middle"
            dominantBaseline="central"
            fill="white"
            fontWeight="bold"
            fontSize="18"
            className="node-key"
          >
            {node.key}
          </text>

          {/* Вес узла (выделенный) */}
          <text
            x={node.x}
            y={weightBoxY + weightBgHeight / 2}
            textAnchor="middle"
            dominantBaseline="central"
            fill={isHighlighted ? "#92400E" : "#1A202C"}
            fontWeight="bold"
            fontSize="14"
            className="node-weight"
          >
            вес: {node.weight}
          </text>

          {/* Рекурсивно рендерим потомков */}
          {renderNode(node.left)}
          {renderNode(node.right)}
        </g>
      );
    };

    if (!tree) return null;

    // Клонируем дерево для модификации позиций
    const cloneTree = (node) => {
      if (!node) return null;
      const cloned = new TreeNode(node.key, node.weight);
      cloned.left = cloneTree(node.left);
      cloned.right = cloneTree(node.right);
      return cloned;
    };

    const positionedTree = cloneTree(tree);
    const height = getTreeHeight(positionedTree);
    const nodeCount = getNodeCount(positionedTree);
    
    // Вычисляем размеры SVG (учитываем увеличенный вертикальный отступ)
    const svgWidth = Math.max(1200, nodeCount * 200);
    const svgHeight = height * 140 + 250;
    
    // Размещаем узлы
    const maxX = calculatePositions(positionedTree, 100, 50, 0, 200);
    const treeWidth = maxX - 100;
    const centerOffset = (svgWidth - treeWidth) / 2 - 100;
    
    // Центрируем дерево
    centerTree(positionedTree, centerOffset);
    
    // Находим минимальную X координату для корректного отображения
    const findMinX = (node, minX = Infinity) => {
      if (!node) return minX;
      if (node.x < minX) minX = node.x;
      minX = findMinX(node.left, minX);
      minX = findMinX(node.right, minX);
      return minX;
    };
    
    const minX = findMinX(positionedTree);
    if (minX < 50) {
      centerTree(positionedTree, 50 - minX);
    }

    return (
      <div className="tree-container">
        <h3>Почти оптимальное дерево поиска ({algo || algorithm})</h3>
        <div className="tree-svg-wrapper">
          <svg 
            width={svgWidth} 
            height={svgHeight} 
            className="tree-svg"
            viewBox={`0 0 ${svgWidth} ${svgHeight}`}
            preserveAspectRatio="xMidYTop meet"
          >
            {/* Градиент для узлов */}
            <defs>
              <linearGradient id="nodeGradient" x1="0%" y1="0%" x2="0%" y2="100%">
                <stop offset="0%" stopColor="#63B3ED" />
                <stop offset="100%" stopColor="#4299E1" />
              </linearGradient>
            </defs>
            {renderNode(positionedTree)}
          </svg>
          {/* Overlay для логирования в правом нижнем углу */}
          <div className="tree-log-overlay">
            <StepVisualization />
          </div>
        </div>
      </div>
    );
  };

  // Обновление состояния при изменении шага
  useEffect(() => {
    if (stepByStep.length === 0) {
      setAnimatedNodes(new Set());
      setHighlightedNode(null);
      return;
    }
    
    const current = stepByStep[currentStep];
    if (!current) return;

    // Собираем все вставленные узлы до текущего шага
    const newAnimatedNodes = new Set();
    for (let i = 0; i <= currentStep; i++) {
      if (stepByStep[i]?.insertedKey) {
        newAnimatedNodes.add(stepByStep[i].insertedKey);
      }
    }
    setAnimatedNodes(newAnimatedNodes);

    // Обновляем подсветку
    if (current.highlightedKey) {
      setHighlightedNode(current.highlightedKey);
    } else if (current.action !== "build_children" && current.action !== "create_root") {
      setHighlightedNode(null);
    }
  }, [currentStep, stepByStep]);

  const StepVisualization = () => {
    if (stepByStep.length === 0) return null;

    const current = stepByStep[currentStep];
    const progress = ((currentStep + 1) / stepByStep.length) * 100;

    const handleStepChange = (newStep) => {
      stopAnimation();
      setCurrentStep(newStep);
      const newAnimatedNodes = new Set();
      for (let i = 0; i <= newStep; i++) {
        if (stepByStep[i].insertedKey) {
          newAnimatedNodes.add(stepByStep[i].insertedKey);
        }
      }
      setAnimatedNodes(newAnimatedNodes);
      setHighlightedNode(stepByStep[newStep]?.highlightedKey || null);
    };

    return (
      <div className="steps-overlay">
        <div className="steps-header">
          <span className="steps-title">📋 Логирование процесса</span>
          <span className="steps-progress">Шаг {currentStep + 1} из {stepByStep.length} ({progress.toFixed(1)}%)</span>
        </div>
        <div className="progress-bar-container">
          <div className="progress-bar" style={{ width: `${progress}%` }}></div>
        </div>
        
        <div className="step-content">
          <div className="step-action-badge">{current.action}</div>
          
          <div className="step-message-full">
            <pre className="step-message-text-full">{current.message}</pre>
          </div>

          {current.checkingKeys && current.checkingKeys.length > 0 && (
            <div className="checking-keys-full">
              <strong>Проверяемые ключи:</strong> {current.checkingKeys.join(", ")}
            </div>
          )}

          {current.totalWeight !== undefined && (
            <div className="step-details-full">
              <div className="detail-item">
                <span className="detail-label">Сумма весов:</span>
                <span className="detail-value">{current.totalWeight}</span>
              </div>
              {current.halfWeight !== undefined && (
                <div className="detail-item">
                  <span className="detail-label">Половинная сумма:</span>
                  <span className="detail-value">{current.halfWeight.toFixed(2)}</span>
                </div>
              )}
              {current.prevSum !== undefined && (
                <div className="detail-item">
                  <span className="detail-label">Сумма слева:</span>
                  <span className="detail-value">{current.prevSum.toFixed(2)}</span>
                </div>
              )}
              {current.newSum !== undefined && (
                <div className="detail-item">
                  <span className="detail-label">Сумма после:</span>
                  <span className="detail-value">{current.newSum.toFixed(2)}</span>
                </div>
              )}
            </div>
          )}

          {current.key && (
            <div className="step-key-info">
              <strong>Текущий ключ:</strong> <span className="key-value">{current.key}</span>
              {current.weight && <span className="weight-value"> (вес: {current.weight})</span>}
            </div>
          )}
        </div>

        <div className="step-controls-full">
          <button
            onClick={stopAnimation}
            className="control-button-full stop-button"
            disabled={!isAnimating}
            title="Остановить анимацию"
          >
            ⏹ Стоп
          </button>
          <button
            onClick={() => handleStepChange(Math.max(0, currentStep - 1))}
            disabled={currentStep === 0 || isAnimating}
            className="control-button-full"
            title="Предыдущий шаг"
          >
            ← Назад
          </button>
          <button
            onClick={startAnimation}
            className="control-button-full play-button"
            disabled={isAnimating}
            title="Автоматическая анимация"
          >
            ▶ Авто
          </button>
          <button
            onClick={() => handleStepChange(Math.min(stepByStep.length - 1, currentStep + 1))}
            disabled={currentStep === stepByStep.length - 1 || isAnimating}
            className="control-button-full"
            title="Следующий шаг"
          >
            Вперед →
          </button>
        </div>

        <div className="step-navigation">
          <input
            type="range"
            min="0"
            max={stepByStep.length - 1}
            value={currentStep}
            onChange={(e) => handleStepChange(Number(e.target.value))}
            className="step-slider"
            disabled={isAnimating}
          />
          <div className="step-slider-labels">
            <span>Начало</span>
            <span>Конец</span>
          </div>
        </div>
      </div>
    );
  };

  return (
    <div className="app">
      <header className="app-header">
        <h1>Визуализация алгоритмов A1 и A2</h1>
        <p>Построение почти оптимальных деревьев поиска</p>
      </header>

      <div className="controls">
        <div className="input-group">
          <label>Количество чисел: <strong>{nodeCount}</strong></label>
          <input
            type="range"
            min="1"
            max="100"
            value={nodeCount}
            onChange={(e) => {
              setNodeCount(Number(e.target.value));
              stopAnimation();
            }}
            className="slider"
          />
          <div className="slider-labels">
            <span>1</span>
            <span>100</span>
          </div>
        </div>

        <div className="algorithm-selector">
          <label>Алгоритм:</label>
          <select
            value={algorithm}
            onChange={(e) => {
              setAlgorithm(e.target.value);
              stopAnimation();
            }}
          >
            <option value="A1">A1 - Сортировка по весам</option>
            <option value="A2">A2 - Балансировка по весам (почти оптимальное)</option>
          </select>
        </div>

        <button onClick={buildTree} className="build-button">
          🔄 Перестроить дерево
        </button>
      </div>

      <div className="main-content">
        <div className="left-panel">
          <div className="weights-info">
            <h3>Числа и их веса</h3>
            <div className="weights-grid">
              {Object.entries(weights)
                .sort(([a], [b]) => Number(a) - Number(b))
                .map(([key, weight]) => (
                <div key={key} className="weight-item">
                  <span className="letter">{key}</span>
                  <span className="count">{weight}</span>
                </div>
              ))}
            </div>
          </div>

          <div className="algorithm-info">
            <h3>Описание алгоритмов</h3>
            <div className="algo-description">
              <strong>Алгоритм A1:</strong> Сортировка вершин по убыванию весов и
              последовательная вставка в дерево поиска
            </div>
            <div className="algo-description">
              <strong>Алгоритм A2:</strong> Рекурсивное построение с выбором корня,
              разделяющего суммарный вес пополам
            </div>
          </div>
        </div>

        <div className="visualization-container">
          <TreeVisualization 
            tree={algorithm === "A1" ? a1Tree : a2Tree}
            animatedNodes={animatedNodes}
            highlightedNode={highlightedNode}
            isAnimating={isAnimating}
            algorithm={algorithm}
          />
        </div>
      </div>
    </div>
  );
};

export default App;
