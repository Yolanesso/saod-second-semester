// App.jsx
import React, { useState, useEffect } from "react";
import "./App.css";

// Компонент визуализации дерева
const Tree = ({ data }) => {
  const TreeNode = ({ node, x, y, level, maxLevel }) => {
    if (!node) return null;

    // Динамическое расстояние между узлами в зависимости от уровня
    const horizontalSpacing = 280 / (level + 1);
    const verticalSpacing = 120;

    const leftX = x - horizontalSpacing;
    const rightX = x + horizontalSpacing;
    const nextY = y + verticalSpacing;

    return (
      <>
        {/* Линии к детям */}
        {node.left && (
          <line
            x1={x}
            y1={y + 30}
            x2={leftX}
            y2={nextY - 30}
            stroke="#4A90E2"
            strokeWidth="3"
          />
        )}
        {node.right && (
          <line
            x1={x}
            y1={y + 30}
            x2={rightX}
            y2={nextY - 30}
            stroke="#4A90E2"
            strokeWidth="3"
          />
        )}

        {/* Узел */}
        <g>
          <circle
            cx={x}
            cy={y}
            r="35"
            fill="#fff"
            stroke="#4A90E2"
            strokeWidth="4"
          />
          <text
            x={x}
            y={y - 10}
            textAnchor="middle"
            fontSize="18"
            fontWeight="bold"
            fill="#2C3E50"
          >
            {node.key}
          </text>
          <text
            x={x}
            y={y + 15}
            textAnchor="middle"
            fontSize="14"
            fill="#7F8C8D"
          >
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

  const calculateTreeHeight = (node) => {
    if (!node) return 0;
    return (
      1 +
      Math.max(calculateTreeHeight(node.left), calculateTreeHeight(node.right))
    );
  };

  const height = calculateTreeHeight(data);
  const svgWidth = 1600;
  const svgHeight = height * 140 + 150;

  return (
    <div className="tree-container">
      <div className="tree-svg-wrapper">
        <svg
          width="100%"
          height={svgHeight}
          viewBox={`0 0 ${svgWidth} ${svgHeight}`}
          preserveAspectRatio="xMidYMin meet"
        >
          <TreeNode
            node={data}
            x={svgWidth / 2}
            y={100}
            level={0}
            maxLevel={height}
          />
        </svg>
      </div>
    </div>
  );
};

// Компонент таблицы матрицы
const MatrixTable = ({ title, data, type }) => {
  if (!data || data.length === 0) {
    return (
      <div className="matrix-table">
        <h3>{title}</h3>
        <div className="matrix-empty">Матрица не сгенерирована</div>
      </div>
    );
  }

  const getCellColor = (value, row, col, type) => {
    if (type === "AR" && row < col) {
      return "#E8F5E8";
    }
    if (type === "AP" && row === 0 && col === data.length - 1) {
      return "#FFF3CD";
    }
    if (row === col) return "#F8F9FA";
    return "#FFFFFF";
  };

  // Отображаем больше элементов для больших экранов
  const displayData = data
    .slice(0, Math.min(16, data.length))
    .map((row) => row.slice(0, Math.min(16, data.length)));

  return (
    <div className="matrix-table">
      <h3>{title}</h3>
      {data.length > 16 && (
        <div className="matrix-warning">
          Показаны первые 15x15 элементов из {data.length - 1}x{data.length - 1}
        </div>
      )}
      <div className="matrix-container">
        <table>
          <thead>
            <tr>
              <th className="corner">i\j</th>
              {displayData[0].map((_, index) => (
                <th key={index}>{index}</th>
              ))}
            </tr>
          </thead>
          <tbody>
            {displayData.map((row, i) => (
              <tr key={i}>
                <th>{i}</th>
                {row.map((cell, j) => (
                  <td
                    key={j}
                    style={{
                      backgroundColor: getCellColor(cell, i, j, type),
                    }}
                    className={i > j ? "empty-cell" : "data-cell"}
                    title={`[${i},${j}] = ${cell}`}
                  >
                    <span className="cell-value">{i > j ? "×" : cell}</span>
                  </td>
                ))}
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
};

// Компонент управления
const Controls = ({ nodeCount, setNodeCount, onGenerate, isLoading }) => {
  return (
    <div className="controls">
      <div className="control-group">
        <label htmlFor="nodeCount">
          Количество вершин: <strong>{nodeCount}</strong>
        </label>
        <input
          id="nodeCount"
          type="range"
          min="3"
          max="20"
          value={nodeCount}
          onChange={(e) => setNodeCount(parseInt(e.target.value))}
        />
        <div className="range-labels">
          <span>3</span>
          <span>20</span>
        </div>
      </div>

      <button
        onClick={onGenerate}
        disabled={isLoading}
        className="generate-btn"
      >
        {isLoading ? (
          <>
            <span className="spinner"></span>
            Генерация...
          </>
        ) : (
          "🎲 Сгенерировать новое дерево"
        )}
      </button>

      <div className="stats">
        <div className="stat-item">
          <span className="stat-label">Максимум вершин:</span>
          <span className="stat-value">20</span>
        </div>
        <div className="stat-item">
          <span className="stat-label">Диапазон весов:</span>
          <span className="stat-value">1-100</span>
        </div>
      </div>
    </div>
  );
};

// Алгоритмы
const generateRandomWeights = (n) => {
  return Array.from({ length: n }, () => Math.floor(Math.random() * 100) + 1);
};

const generateOptimalBST = (weights) => {
  const n = weights.length;

  // Инициализация матриц
  const AW = Array(n + 1)
    .fill()
    .map(() => Array(n + 1).fill(0));
  const AP = Array(n + 1)
    .fill()
    .map(() => Array(n + 1).fill(0));
  const AR = Array(n + 1)
    .fill()
    .map(() => Array(n + 1).fill(0));

  // Матрица весов AW
  for (let i = 0; i <= n; i++) {
    AW[i][i] = 0;
    for (let j = i + 1; j <= n; j++) {
      AW[i][j] = AW[i][j - 1] + weights[j - 1];
    }
  }

  // Поддеревья из одного ключа
  for (let i = 0; i < n; i++) {
    const j = i + 1;
    AP[i][j] = AW[i][j];
    AR[i][j] = j;
  }

  // Поддеревья из h ключей
  for (let h = 2; h <= n; h++) {
    for (let i = 0; i <= n - h; i++) {
      const j = i + h;
      let m = AR[i][j - 1];
      let minVal = AP[i][m - 1] + AP[m][j];

      const maxK = AR[i + 1][j] || j;
      for (let k = m + 1; k <= maxK; k++) {
        const cost = AP[i][k - 1] + AP[k][j];
        if (cost < minVal) {
          m = k;
          minVal = cost;
        }
      }

      AP[i][j] = minVal + AW[i][j];
      AR[i][j] = m;
    }
  }

  // Построение дерева
  const buildTree = (L, R) => {
    if (L >= R) return null;

    const k = AR[L][R];
    const node = {
      key: k,
      weight: weights[k - 1],
      left: buildTree(L, k - 1),
      right: buildTree(k, R),
    };

    return node;
  };

  const tree = buildTree(0, n);

  return {
    tree,
    AW,
    AP,
    AR,
    weights,
  };
};

// Основной компонент приложения
const App = () => {
  const [treeData, setTreeData] = useState(null);
  const [matrices, setMatrices] = useState({ AW: [], AP: [], AR: [] });
  const [nodeCount, setNodeCount] = useState(12);
  const [isLoading, setIsLoading] = useState(false);

  const generateTree = () => {
    setIsLoading(true);
    setTimeout(() => {
      const weights = generateRandomWeights(nodeCount);
      const result = generateOptimalBST(weights);
      setTreeData(result.tree);
      setMatrices({
        AW: result.AW,
        AP: result.AP,
        AR: result.AR,
      });
      setIsLoading(false);
    }, 300);
  };

  useEffect(() => {
    generateTree();
  }, []);

  return (
    <div className="app">
      <header className="app-header">
        <div className="header-content">
          <h1>🌳 Визуализатор Деревьева Оптимального Поиска</h1>
          <p>Точный алгоритм построения ДОП</p>
        </div>
      </header>

      <div className="app-body">
        <aside className="sidebar">
          <Controls
            nodeCount={nodeCount}
            setNodeCount={setNodeCount}
            onGenerate={generateTree}
            isLoading={isLoading}
          />
        </aside>

        <main className="main-content">
          <section className="tree-section">
            <div className="section-header">
              <h2>📊 Дерево оптимального поиска</h2>
              {treeData && (
                <div className="tree-info">
                  <span>Вершин: {nodeCount}</span>
                  <span>Корень: {treeData.key}</span>
                </div>
              )}
            </div>
            {treeData ? (
              <Tree data={treeData} />
            ) : (
              <div className="loading">Загрузка дерева...</div>
            )}
          </section>

          <section className="matrices-section">
            <h2>🧮 Матрицы алгоритма</h2>
            <div className="matrices-grid">
              <MatrixTable
                title="📋 Матрица весов (AW)"
                data={matrices.AW}
                type="AW"
              />
              <MatrixTable
                title="💰 Матрица стоимостей (AP)"
                data={matrices.AP}
                type="AP"
              />
              <MatrixTable
                title="⭐ Матрица корней (AR)"
                data={matrices.AR}
                type="AR"
              />
            </div>
          </section>
        </main>
      </div>
    </div>
  );
};

export default App;
