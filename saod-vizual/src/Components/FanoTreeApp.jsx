import React, { useState, useMemo } from 'react';
import { buildFanoTree, encodeText } from './FanoTree';
import FanoTreeVisualization from './FanoTreeVisualization';
import '../css/FanoTreeApp.css';

function FanoTreeApp() {
  const [text, setText] = useState('hello world');
  const [showStats, setShowStats] = useState(true);

  const treeData = useMemo(() => {
    if (!text || text.trim().length === 0) {
      return null;
    }
    return buildFanoTree(text);
  }, [text]);

  const encodedText = useMemo(() => {
    if (!text || !treeData) return '';
    return encodeText(text, treeData.encodingTable);
  }, [text, treeData]);

  const compressionRatio = useMemo(() => {
    if (!text || !encodedText) return 0;
    const originalBits = text.length * 8;
    const compressedBits = encodedText.length;
    return originalBits > 0 ? ((1 - compressedBits / originalBits) * 100).toFixed(2) : 0;
  }, [text, encodedText]);

  const handleTextChange = (e) => {
    setText(e.target.value);
  };

  const handleExample = (exampleText) => {
    setText(exampleText);
  };

  return (
    <div className="fano-app">
      <div className="fano-header">
        <h1>🌳 Дерево Фано (Хаффмана)</h1>
        <p>Визуализация алгоритма сжатия данных</p>
      </div>

      <div className="fano-body">
        <div className="fano-sidebar">
          <div className="input-section">
            <label htmlFor="text-input" className="input-label">
              Введите текст для анализа:
            </label>
            <textarea
              id="text-input"
              className="text-input"
              value={text}
              onChange={handleTextChange}
              placeholder="Введите текст здесь..."
              rows="6"
            />
            <div className="char-count">
              Символов: {text.length}
            </div>
          </div>

          <div className="examples-section">
            <div className="examples-label">Примеры:</div>
            <div className="examples-buttons">
              <button
                className="example-btn"
                onClick={() => handleExample('hello world')}
              >
                hello world
              </button>
              <button
                className="example-btn"
                onClick={() => handleExample('абракадабра')}
              >
                абракадабра
              </button>
              <button
                className="example-btn"
                onClick={() => handleExample('mississippi')}
              >
                mississippi
              </button>
              <button
                className="example-btn"
                onClick={() => handleExample('aaaaaaaaaabbbbbccccc')}
              >
                aaaaaaaaabbbbbccccc
              </button>
            </div>
          </div>

          {treeData && (
            <div className="stats-section">
              <div className="stats-header">
                <h3>Статистика</h3>
                <button
                  className="toggle-btn"
                  onClick={() => setShowStats(!showStats)}
                >
                  {showStats ? '▼' : '▶'}
                </button>
              </div>
              
              {showStats && (
                <div className="stats-content">
                  <div className="stat-item">
                    <span className="stat-label">Уникальных символов:</span>
                    <span className="stat-value">{treeData.stats.length}</span>
                  </div>
                  <div className="stat-item">
                    <span className="stat-label">Сжатие:</span>
                    <span className="stat-value">{compressionRatio}%</span>
                  </div>
                  <div className="stat-item">
                    <span className="stat-label">Исходный размер:</span>
                    <span className="stat-value">{text.length * 8} бит</span>
                  </div>
                  <div className="stat-item">
                    <span className="stat-label">Сжатый размер:</span>
                    <span className="stat-value">{encodedText.length} бит</span>
                  </div>
                  
                  <div className="encoding-table">
                    <h4>Таблица кодирования:</h4>
                    <div className="table-scroll">
                      <table>
                        <thead>
                          <tr>
                            <th>Символ</th>
                            <th>Частота</th>
                            <th>Код</th>
                            <th>Длина</th>
                          </tr>
                        </thead>
                        <tbody>
                          {treeData.stats.map((stat, idx) => (
                            <tr key={idx}>
                              <td className="char-cell">
                                {stat.displayChar}
                              </td>
                              <td>{stat.frequency}</td>
                              <td className="code-cell">{stat.code}</td>
                              <td>{stat.codeLength}</td>
                            </tr>
                          ))}
                        </tbody>
                      </table>
                    </div>
                  </div>
                </div>
              )}
            </div>
          )}
        </div>

        <div className="fano-main">
          <div className="visualization-section">
            <div className="section-header">
              <h2>Визуализация дерева</h2>
              {treeData && (
                <div className="tree-info">
                  <span>Узлов: {countNodes(treeData.root)}</span>
                  <span>Листьев: {treeData.stats.length}</span>
                  <span>Внутренних: {countNodes(treeData.root) - treeData.stats.length}</span>
                </div>
              )}
            </div>
            <div className="tree-container">
              <FanoTreeVisualization treeData={treeData} />
            </div>
          </div>

          {treeData && encodedText && (
            <div className="encoding-section">
              <h3>Закодированный текст:</h3>
              <div className="encoded-text">
                {encodedText}
              </div>
              <div className="encoded-info">
                Длина: {encodedText.length} бит (было {text.length * 8} бит)
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

function countNodes(node) {
  if (!node) return 0;
  return 1 + countNodes(node.left) + countNodes(node.right);
}

export default FanoTreeApp;

