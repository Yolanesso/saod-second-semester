// Алгоритм построения дерева Фано (Хаффмана)

class TreeNode {
  constructor(char, frequency, left = null, right = null) {
    this.char = char;
    this.frequency = frequency;
    this.left = left;
    this.right = right;
    this.code = '';
  }

  isLeaf() {
    return !this.left && !this.right;
  }
}

export function buildFanoTree(text) {
  if (!text || text.trim().length === 0) {
    return null;
  }

  // Подсчет частоты символов
  const frequencyMap = new Map();
  for (const char of text) {
    frequencyMap.set(char, (frequencyMap.get(char) || 0) + 1);
  }

  // Создание узлов - гарантируем уникальность символов
  // Сортируем по возрастанию частоты для правильного алгоритма Хаффмана
  const nodes = Array.from(frequencyMap.entries())
    .map(([char, freq]) => new TreeNode(char, freq))
    .sort((a, b) => a.frequency - b.frequency);
  
  // Проверка: каждый символ должен быть уникальным
  const uniqueChars = new Set(nodes.map(n => n.char).filter(c => c !== null));
  if (uniqueChars.size !== nodes.filter(n => n.char !== null).length) {
    console.warn('Обнаружены дубликаты символов при построении дерева!');
  }

  // Построение дерева Хаффмана
  // Для двоичного дерева нужно минимум 2 уникальных символа
  if (nodes.length < 2) {
    // Если меньше 2 символов, дерево Хаффмана не может быть построено
    console.warn('Для построения дерева Хаффмана нужно минимум 2 уникальных символа');
    return null;
  }
  
  while (nodes.length > 1) {
    // Берем два узла с наименьшей частотой (первые в отсортированном массиве)
    const left = nodes.shift();
    const right = nodes.shift();

    // Создаем новый узел - всегда с двумя детьми для двоичного дерева
    const merged = new TreeNode(
      null,
      left.frequency + right.frequency,
      left,
      right
    );

    // Вставляем обратно в массив, сохраняя порядок по возрастанию частоты
    let insertIndex = nodes.length;
    for (let i = 0; i < nodes.length; i++) {
      if (nodes[i].frequency >= merged.frequency) {
        insertIndex = i;
        break;
      }
    }
    nodes.splice(insertIndex, 0, merged);
  }

  const root = nodes[0];

  // Назначение кодов
  assignCodes(root, '');

  // Получение таблицы кодирования
  const encodingTable = new Map();
  const stats = [];
  collectCodes(root, encodingTable, stats);

  return {
    root,
    encodingTable,
    stats: stats.sort((a, b) => b.frequency - a.frequency),
    frequencyMap,
  };
}

function assignCodes(node, code) {
  if (!node) return;
  node.code = code;
  if (node.left) assignCodes(node.left, code + '0');
  if (node.right) assignCodes(node.right, code + '1');
}

function collectCodes(node, table, stats) {
  if (!node) return;
  if (node.isLeaf()) {
    const char = node.char === ' ' ? '␣' : node.char === '\n' ? '\\n' : node.char;
    table.set(node.char, node.code);
    stats.push({
      char: node.char,
      displayChar: char,
      frequency: node.frequency,
      code: node.code,
      codeLength: node.code.length,
    });
  } else {
    collectCodes(node.left, table, stats);
    collectCodes(node.right, table, stats);
  }
}

export function encodeText(text, encodingTable) {
  if (!text || !encodingTable) return '';
  
  let encoded = '';
  for (const char of text) {
    const code = encodingTable.get(char);
    if (code !== undefined) {
      encoded += code;
    }
  }
  return encoded;
}

