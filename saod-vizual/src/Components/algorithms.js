// utils/algorithms.js
// Алгоритмы построения ДОП

export const generateRandomWeights = (n) => {
  return Array.from({ length: n }, () => Math.floor(Math.random() * 100) + 1);
};

export const generateOptimalBST = (weights) => {
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

      for (let k = m + 1; k <= AR[i + 1][j]; k++) {
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
