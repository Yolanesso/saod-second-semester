// App.js
import React, { useState, useRef, useEffect } from "react";
import "../css/Avl.css";

// Узел дерева
class TreeNode {
  constructor(value) {
    this.value = value;
    this.left = null;
    this.right = null;
    this.height = 1;
    this.id = Math.random().toString(36).substr(2, 9);
  }
}

// АВЛ-дерево
class AVLTree {
  constructor() {
    this.root = null;
    this.operations = [];
  }

  // Получить высоту узла
  getHeight(node) {
    return node ? node.height : 0;
  }

  // Получить баланс-фактор узла
  getBalance(node) {
    return node ? this.getHeight(node.left) - this.getHeight(node.right) : 0;
  }

  // Правый поворот
  rightRotate(y) {
    const x = y.left;
    const T2 = x.right;

    // Выполняем поворот
    x.right = y;
    y.left = T2;

    // Обновляем высоты
    y.height = Math.max(this.getHeight(y.left), this.getHeight(y.right)) + 1;
    x.height = Math.max(this.getHeight(x.left), this.getHeight(x.right)) + 1;

    // Записываем операцию
    this.operations.push({
      type: "rotate",
      node: y.value,
      direction: "right",
      newRoot: x.value,
    });

    return x;
  }

  // Левый поворот
  leftRotate(x) {
    const y = x.right;
    const T2 = y.left;

    // Выполняем поворот
    y.left = x;
    x.right = T2;

    // Обновляем высоты
    x.height = Math.max(this.getHeight(x.left), this.getHeight(x.right)) + 1;
    y.height = Math.max(this.getHeight(y.left), this.getHeight(y.right)) + 1;

    // Записываем операцию
    this.operations.push({
      type: "rotate",
      node: x.value,
      direction: "left",
      newRoot: y.value,
    });

    return y;
  }

  // Вставка узла
  insert(node, value) {
    // Обычная вставка в BST
    if (!node) {
      this.operations.push({
        type: "insert",
        value: value,
      });
      return new TreeNode(value);
    }

    if (value < node.value) {
      node.left = this.insert(node.left, value);
    } else if (value > node.value) {
      node.right = this.insert(node.right, value);
    } else {
      return node; // Дубликаты не допускаются
    }

    // Обновляем высоту текущего узла
    node.height =
      1 + Math.max(this.getHeight(node.left), this.getHeight(node.right));

    // Получаем баланс-фактор
    const balance = this.getBalance(node);

    // Левый левый случай
    if (balance > 1 && value < node.left.value) {
      return this.rightRotate(node);
    }

    // Правый правый случай
    if (balance < -1 && value > node.right.value) {
      return this.leftRotate(node);
    }

    // Левый правый случай
    if (balance > 1 && value > node.left.value) {
      node.left = this.leftRotate(node.left);
      return this.rightRotate(node);
    }

    // Правый левый случай
    if (balance < -1 && value < node.right.value) {
      node.right = this.rightRotate(node.right);
      return this.leftRotate(node);
    }

    return node;
  }

  // Общедоступный метод вставки
  insertValue(value) {
    this.operations = [];
    this.root = this.insert(this.root, value);
    return this.operations;
  }

  // Поиск минимального узла
  minValueNode(node) {
    let current = node;
    while (current.left) {
      current = current.left;
    }
    return current;
  }

  // Удаление узла
  deleteNode(node, value) {
    if (!node) {
      return node;
    }

    if (value < node.value) {
      node.left = this.deleteNode(node.left, value);
    } else if (value > node.value) {
      node.right = this.deleteNode(node.right, value);
    } else {
      // Узел с одним или без детей
      if (!node.left || !node.right) {
        const temp = node.left || node.right;

        // Нет детей
        if (!temp) {
          node = null;
        } else {
          // Один ребенок
          node = temp;
        }
      } else {
        // Узел с двумя детьми
        const temp = this.minValueNode(node.right);
        node.value = temp.value;
        node.right = this.deleteNode(node.right, temp.value);
      }
    }

    // Если дерево стало пустым
    if (!node) {
      return node;
    }

    // Обновляем высоту текущего узла
    node.height =
      1 + Math.max(this.getHeight(node.left), this.getHeight(node.right));

    // Получаем баланс-фактор
    const balance = this.getBalance(node);

    // Левый левый случай
    if (balance > 1 && this.getBalance(node.left) >= 0) {
      return this.rightRotate(node);
    }

    // Левый правый случай
    if (balance > 1 && this.getBalance(node.left) < 0) {
      node.left = this.leftRotate(node.left);
      return this.rightRotate(node);
    }

    // Правый правый случай
    if (balance < -1 && this.getBalance(node.right) <= 0) {
      return this.leftRotate(node);
    }

    // Правый левый случай
    if (balance < -1 && this.getBalance(node.right) > 0) {
      node.right = this.rightRotate(node.right);
      return this.leftRotate(node);
    }

    return node;
  }

  // Общедоступный метод удаления
  deleteValue(value) {
    this.operations = [];
    this.root = this.deleteNode(this.root, value);
    return this.operations;
  }

  // Поиск узла
  find(node, value) {
    if (!node) {
      return false;
    }

    if (value === node.value) {
      return true;
    } else if (value < node.value) {
      return this.find(node.left, value);
    } else {
      return this.find(node.right, value);
    }
  }

  // Общедоступный метод поиска
  findValue(value) {
    return this.find(this.root, value);
  }

  // Обход дерева в порядке (для отображения)
  inOrder(node, result = []) {
    if (node) {
      this.inOrder(node.left, result);
      result.push(node.value);
      this.inOrder(node.right, result);
    }
    return result;
  }

  // Получить дерево в виде объекта для отрисовки
  getTreeForRender(node = this.root) {
    if (!node) return null;

    return {
      id: node.id,
      value: node.value,
      height: node.height,
      balance: this.getBalance(node),
      left: this.getTreeForRender(node.left),
      right: this.getTreeForRender(node.right),
    };
  }
}

// Компонент для отрисовки узла дерева
const TreeNodeComponent = ({
  node,
  x,
  y,
  level = 0,
  highlighted = false,
  isNew = false,
  isDeleted = false,
}) => {
  const nodeRef = useRef(null);
  const horizontalSpacing = Math.max(200 - level * 20, 80);
  const verticalSpacing = 100;

  useEffect(() => {
    if (nodeRef.current && (isNew || isDeleted)) {
      nodeRef.current.classList.add(isNew ? "node-enter" : "node-exit");

      const timeout = setTimeout(() => {
        if (nodeRef.current) {
          nodeRef.current.classList.remove(isNew ? "node-enter" : "node-exit");
        }
      }, 600);

      return () => clearTimeout(timeout);
    }
  }, [isNew, isDeleted]);

  if (!node) return null;

  return (
    <g>
      {/* Соединительные линии к детям */}
      {node.left && (
        <line
          x1={x}
          y1={y}
          x2={x - horizontalSpacing}
          y2={y + verticalSpacing}
          className="tree-line"
        />
      )}
      {node.right && (
        <line
          x1={x}
          y1={y}
          x2={x + horizontalSpacing}
          y2={y + verticalSpacing}
          className="tree-line"
        />
      )}

      {/* Узел */}
      <circle
        ref={nodeRef}
        cx={x}
        cy={y}
        r={25}
        className={`tree-node ${highlighted ? "highlighted" : ""} ${
          isNew ? "new-node" : ""
        } ${isDeleted ? "deleted-node" : ""}`}
      />

      {/* Значение узла */}
      <text x={x} y={y} textAnchor="middle" dy="0.3em" className="node-value">
        {node.value}
      </text>

      {/* Высота узла */}
      <text x={x} y={y + 35} textAnchor="middle" className="node-height">
        h:{node.height}
      </text>

      {/* Баланс-фактор */}
      <text x={x} y={y + 50} textAnchor="middle" className="node-balance">
        b:{node.balance}
      </text>

      {/* Рекурсивно отрисовываем детей */}
      {node.left && (
        <TreeNodeComponent
          node={node.left}
          x={x - horizontalSpacing}
          y={y + verticalSpacing}
          level={level + 1}
          highlighted={highlighted}
          isNew={isNew}
          isDeleted={isDeleted}
        />
      )}
      {node.right && (
        <TreeNodeComponent
          node={node.right}
          x={x + horizontalSpacing}
          y={y + verticalSpacing}
          level={level + 1}
          highlighted={highlighted}
          isNew={isNew}
          isDeleted={isDeleted}
        />
      )}
    </g>
  );
};

// Основной компонент приложения
const App = () => {
  const [tree, setTree] = useState(new AVLTree());
  const [inputValue, setInputValue] = useState("");
  const [operations, setOperations] = useState([]);
  const [currentStep, setCurrentStep] = useState(0);
  const [highlightedNode, setHighlightedNode] = useState(null);
  const [newNode, setNewNode] = useState(null);
  const [deletedNode, setDeletedNode] = useState(null);
  const [isAnimating, setIsAnimating] = useState(false);
  const [operationHistory, setOperationHistory] = useState([]);
  const [treeScale, setTreeScale] = useState(1);
  const [treePosition, setTreePosition] = useState({ x: 0, y: 0 });
  const [isDragging, setIsDragging] = useState(false);
  const [lastMousePos, setLastMousePos] = useState({ x: 0, y: 0 });

  // Обработчик вставки
  const handleInsert = () => {
    if (!inputValue || isAnimating) return;

    const value = parseInt(inputValue);
    if (isNaN(value)) return;

    const newTree = new AVLTree();
    newTree.root = JSON.parse(JSON.stringify(tree.root));
    const ops = newTree.insertValue(value);

    setOperations(ops);
    setCurrentStep(0);
    setHighlightedNode(null);
    setNewNode(value);
    setDeletedNode(null);
    setIsAnimating(true);

    // Анимация вставки
    setTimeout(() => {
      setTree(newTree);
      setOperationHistory([...operationHistory, { type: "insert", value }]);
      setNewNode(null);
      setIsAnimating(false);
      setInputValue("");
    }, 800);
  };

  // Обработчик удаления
  const handleDelete = () => {
    if (!inputValue || isAnimating) return;

    const value = parseInt(inputValue);
    if (isNaN(value) || !tree.findValue(value)) return;

    const newTree = new AVLTree();
    newTree.root = JSON.parse(JSON.stringify(tree.root));
    const ops = newTree.deleteValue(value);

    setOperations(ops);
    setCurrentStep(0);
    setHighlightedNode(null);
    setNewNode(null);
    setDeletedNode(value);
    setIsAnimating(true);

    // Анимация удаления
    setTimeout(() => {
      setTree(newTree);
      setOperationHistory([...operationHistory, { type: "delete", value }]);
      setDeletedNode(null);
      setIsAnimating(false);
      setInputValue("");
    }, 800);
  };

  // Обработчик поиска
  const handleSearch = () => {
    if (!inputValue || isAnimating) return;

    const value = parseInt(inputValue);
    if (isNaN(value)) return;

    setHighlightedNode(value);
    setIsAnimating(true);

    setTimeout(() => {
      setIsAnimating(false);
    }, 1500);
  };

  // Обработчик сброса
  const handleReset = () => {
    setTree(new AVLTree());
    setOperations([]);
    setCurrentStep(0);
    setHighlightedNode(null);
    setNewNode(null);
    setDeletedNode(null);
    setOperationHistory([]);
    setTreeScale(1);
    setTreePosition({ x: 0, y: 0 });
  };

  // Обработчик масштабирования
  const handleZoom = (delta) => {
    setTreeScale((prev) => Math.min(Math.max(prev + delta, 0.3), 3));
  };

  // Обработчики перетаскивания
  const handleMouseDown = (e) => {
    if (e.button !== 0) return; // Только левая кнопка мыши
    setIsDragging(true);
    setLastMousePos({ x: e.clientX, y: e.clientY });
  };

  const handleMouseMove = (e) => {
    if (!isDragging) return;

    const deltaX = e.clientX - lastMousePos.x;
    const deltaY = e.clientY - lastMousePos.y;

    setTreePosition((prev) => ({
      x: prev.x + deltaX,
      y: prev.y + deltaY,
    }));

    setLastMousePos({ x: e.clientX, y: e.clientY });
  };

  const handleMouseUp = () => {
    setIsDragging(false);
  };

  // Получение данных дерева для отрисовки
  const treeData = tree.getTreeForRender();

  // Рассчитываем центр SVG
  const getSvgCenter = () => {
    return {
      x: window.innerWidth / 2 + treePosition.x,
      y: 200 + treePosition.y,
    };
  };

  const center = getSvgCenter();

  return (
    <div
      className="app-fullscreen"
      onMouseMove={handleMouseMove}
      onMouseUp={handleMouseUp}
      onMouseLeave={handleMouseUp}
    >
      <header className="app-header">
        <div className="header-content">
          <h1>Визуализатор АВЛ-дерева</h1>
          <p>Интерактивная демонстрация балансировки бинарного дерева поиска</p>
        </div>

        <div className="header-controls">
          <div className="zoom-controls">
            <button
              onClick={() => handleZoom(0.1)}
              className="btn btn-zoom"
              disabled={isAnimating}
            >
              <span className="zoom-icon">+</span>
            </button>
            <button
              onClick={() => handleZoom(-0.1)}
              className="btn btn-zoom"
              disabled={isAnimating}
            >
              <span className="zoom-icon">−</span>
            </button>
            <span className="zoom-value">{Math.round(treeScale * 100)}%</span>
          </div>

          <button
            onClick={handleReset}
            className="btn btn-outline"
            disabled={isAnimating}
          >
            Сбросить всё
          </button>
        </div>
      </header>

      <div className="app-main">
        <div className="control-panel">
          <div className="input-section">
            <div className="input-group">
              <input
                type="number"
                value={inputValue}
                onChange={(e) => setInputValue(e.target.value)}
                placeholder="Введите число"
                className="input-field"
                disabled={isAnimating}
                onKeyPress={(e) => {
                  if (e.key === "Enter") handleInsert();
                }}
              />
              <div className="button-group">
                <button
                  onClick={handleInsert}
                  className="btn btn-primary"
                  disabled={isAnimating || !inputValue}
                >
                  Вставить
                </button>
                <button
                  onClick={handleDelete}
                  className="btn btn-danger"
                  disabled={isAnimating || !inputValue}
                >
                  Удалить
                </button>
                <button
                  onClick={handleSearch}
                  className="btn btn-secondary"
                  disabled={isAnimating || !inputValue}
                >
                  Найти
                </button>
              </div>
            </div>
          </div>

          <div className="info-panel">
            <div className="info-grid">
              <div className="info-item">
                <span className="info-label">Количество узлов:</span>
                <span className="info-value">
                  {tree.inOrder(tree.root).length}
                </span>
              </div>
              <div className="info-item">
                <span className="info-label">Высота дерева:</span>
                <span className="info-value">{tree.getHeight(tree.root)}</span>
              </div>
              <div className="info-item">
                <span className="info-label">Баланс корня:</span>
                <span className="info-value">{tree.getBalance(tree.root)}</span>
              </div>
              <div className="info-item">
                <span className="info-label">Масштаб:</span>
                <span className="info-value">
                  {Math.round(treeScale * 100)}%
                </span>
              </div>
            </div>
          </div>
        </div>

        <div
          className="tree-container-fullscreen"
          onMouseDown={handleMouseDown}
          style={{ cursor: isDragging ? "grabbing" : "grab" }}
        >
          {treeData ? (
            <svg width="100%" height="100%" className="tree-svg-fullscreen">
              <g
                transform={`translate(${center.x}, ${center.y}) scale(${treeScale})`}
              >
                <TreeNodeComponent
                  node={treeData}
                  x={0}
                  y={0}
                  highlighted={highlightedNode}
                  isNew={newNode === treeData.value}
                  isDeleted={deletedNode === treeData.value}
                />
              </g>
            </svg>
          ) : (
            <div className="empty-tree">
              <div className="empty-tree-content">
                <h3>Дерево пустое</h3>
                <p>Добавьте первый узел, чтобы начать визуализацию</p>
                <div className="empty-tree-illustration">
                  <div className="tree-icon">🌳</div>
                </div>
              </div>
            </div>
          )}
        </div>
      </div>

      <div className="app-footer"></div>
    </div>
  );
};

export default App;
