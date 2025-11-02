// components/MatrixTable.js
import React from "react";

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
      return "#E8F5E8"; // Зеленый для корней
    }
    if (type === "AP" && row === 0 && col === data.length - 1) {
      return "#FFF3CD"; // Желтый для общей стоимости
    }
    if (row === col) return "#F8F9FA"; // Серый для диагонали
    return "#FFFFFF";
  };

  return (
    <div className="matrix-table">
      <h3>{title}</h3>
      <div className="matrix-container">
        <table>
          <thead>
            <tr>
              <th>i\j</th>
              {data[0].map((_, index) => (
                <th key={index}>{index}</th>
              ))}
            </tr>
          </thead>
          <tbody>
            {data.map((row, i) => (
              <tr key={i}>
                <th>{i}</th>
                {row.map((cell, j) => (
                  <td
                    key={j}
                    style={{
                      backgroundColor: getCellColor(cell, i, j, type),
                    }}
                    className={i > j ? "empty-cell" : ""}
                  >
                    {i > j ? "" : cell}
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

export default MatrixTable;
