// components/Controls.js
import React from "react";

const Controls = ({ nodeCount, setNodeCount, onGenerate, isLoading }) => {
  return (
    <div className="controls">
      <div className="control-group">
        <label htmlFor="nodeCount">Количество вершин: {nodeCount}</label>
        <input
          id="nodeCount"
          type="range"
          min="3"
          max="20"
          value={nodeCount}
          onChange={(e) => setNodeCount(parseInt(e.target.value))}
        />
      </div>

      <button
        onClick={onGenerate}
        disabled={isLoading}
        className="generate-btn"
      >
        {isLoading ? "Генерация..." : "🎲 Сгенерировать новое дерево"}
      </button>

      <div className="info">
        <small>
          Веса генерируются случайно от 1 до 100. Максимум 20 вершин для лучшей
          визуализации.
        </small>
      </div>
    </div>
  );
};

export default Controls;
