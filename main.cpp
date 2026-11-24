#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <cstdlib>



// ДОП 1: порядок графа (число вершин) задаёт пользователь
size_t read_order(){
    int n = 0;
    while (true){
        std::cout << "ВВЕДИТЕ КОЛИЧЕСТВО ВЕРШИН (от 1 до 50): ";
        if (std::cin >> n && n >= 1 && n <= 50) return n;
        if (std::cin.eof()) std::exit(1);
        std::cout << "НЕКОРРЕКТНОЕ ЗНАЧЕНИЕ\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// ДОП 2: разрешить ли петли (0 - нет, 1 - да)
bool read_loops(){
    int v = 0;
    while (true){
        std::cout << "ПЕТЛИ (0 - нет, 1 - да): ";
        if (std::cin >> v && (v == 0 || v == 1)) return v;
        if (std::cin.eof()) std::exit(1);
        std::cout << "НЕКОРРЕКТНОЕ ЗНАЧЕНИЕ\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}



// вывод матрицы смежности 
void print_matrix(const std::vector<std::vector<bool>>& matrix){
    if (matrix.empty()) return;
    std::cout << "МАТРИЦА СМЕЖНОСТИ:\n";
    for (size_t i = 0; i < matrix.size(); ++i){
        for (size_t j = 0; j < matrix.size(); ++j){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}


// генерация матрицы смежности 
std::vector<std::vector<bool>> gen_matrix_s(std::mt19937& gen, size_t size, bool loops){
    std::vector<std::vector<bool>> matrix(size, std::vector<bool>(size, false));

    std::uniform_int_distribution<int> distrib_b(0, 1); 
    for (size_t i = 0; i < matrix.size(); ++i){
        if (loops) matrix[i][i] = distrib_b(gen);   // ДОП 2: петля у вершины
        for (size_t j = i+1; j < matrix.size(); ++j){
            matrix[i][j] = distrib_b(gen);
            matrix[j][i] = matrix[i][j];
        }
    }

    return matrix;
}

// размер графа по матрице смежности 
size_t size_s(const std::vector<std::vector<bool>>& matrix){
    size_t size = 0;
    for (size_t i = 0; i < matrix.size(); ++i){
        for (size_t j = i; j < matrix.size(); ++j){   // j = i: диагональ, то есть петли (доп. задание 2)
            size += matrix[i][j];
        }
    }
    return size;
}


// матрица инцидентности по матрице смежности 
std::vector<std::vector<bool>> gen_matrix_i(size_t g_cnt, size_t e_cnt, const std::vector<std::vector<bool>>& matrix_s){
    std::vector<std::vector<bool>> matrix_i(g_cnt, std::vector<bool>(e_cnt, 0));
    
    size_t cnt = 0;
    for (size_t i = 0; i < matrix_s.size(); ++i){
        for (size_t j = i; j < matrix_s.size(); ++j){   // j = i: петля, в её столбце одна единица (доп 2)
            if (matrix_s[i][j]) {
                matrix_i[i][cnt] = true;
                matrix_i[j][cnt] = true;
                ++cnt;    
            }
        }
    }

    return matrix_i;
}

// вывод матрицы инцидентности 
void print_matrix_i(const std::vector<std::vector<bool>>& matrix){
    if (matrix.empty()) return;
    size_t n = matrix.size();
    size_t m = matrix[0].size();
    std::cout << "\nМАТРИЦА ИНЦИДЕНТНОСТИ\n";
    std::cout << "КОЛИЧЕСТВО ВЕРШИН: " << n << "\n";
    std::cout << "КОЛИЧЕСТВО РЕБЕР: " << m << "\n";

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

// размер графа по матрице инцидентности 
size_t size_i(const std::vector<std::vector<bool>>& matrix){
    return matrix[0].size();
}


// ДОП 2 у каждой вершины считаем соседей (без петли) и петлю - по матрице смежности
void count_s(const std::vector<std::vector<bool>>& matrix, std::vector<size_t>& neigh, std::vector<size_t>& loop){
    neigh.assign(matrix.size(), 0);
    loop.assign(matrix.size(), 0);
    for (size_t i = 0; i < matrix.size(); ++i){
        for (size_t j = 0; j < matrix.size(); ++j){
            if (!matrix[i][j]) continue;
            if (i == j) loop[i] = 1;
            else ++neigh[i];
        }
    }
}

// то же самое по матрице инцидентности: ребро с одним концом - петля, с двумя - обычное ребро
void count_i(const std::vector<std::vector<bool>>& matrix, std::vector<size_t>& neigh, std::vector<size_t>& loop){
    neigh.assign(matrix.size(), 0);
    loop.assign(matrix.size(), 0);
    if (matrix.empty()) return;
    for (size_t k = 0; k < matrix[0].size(); ++k){
        size_t ends = 0;
        for (size_t i = 0; i < matrix.size(); ++i) ends += matrix[i][k];
        for (size_t i = 0; i < matrix.size(); ++i){
            if (!matrix[i][k]) continue;
            if (ends == 1) loop[i] = 1;
            else ++neigh[i];
        }
    }
}

// изолированные, концевые, доминирующие вершины 
void type_v(const std::vector<size_t>& neigh, const std::vector<size_t>& loop){
    if (neigh.empty()) return;
    size_t n = neigh.size();
    size_t s = 0;

    std::cout << "ПОИСК ОСОБЕННЫХ ВЕРШИН\n";
    for (size_t i = 0; i < n; ++i){
        size_t deg = neigh[i] + 2 * loop[i];   // петля даёт степень 2
        if (deg == 0){
            ++s;
            std::cout << "вершина " << i+1 << " ИЗОЛИРОВАНА\n";
        }
        if (deg == 1) {
            ++s;
            std::cout << "вершина " << i+1 << " КОНЦЕВАЯ\n";
        }
        if (n > 1 && neigh[i] == n-1) {   // смежна со всеми остальными, петля не важна
            ++s;
            std::cout << "вершина " << i+1 << " ДОМИНИРУЮЩАЯ\n";
        }
    }
    if (s == 0) std::cout << "ВЕРШИНЫ НЕ НАЙДЕНЫ :(\n";
}



int main(){
    std::random_device rd;
    std::mt19937 gen(rd());

    size_t n = read_order();     // доп1
    bool loops = read_loops();   // доп 2
    std::vector<size_t> neigh, loop;

    // task1
    std::vector<std::vector<bool>> g_matrix_s = gen_matrix_s(gen, n, loops);   // 1.1
    size_t e_cnt = size_s(g_matrix_s);   // 1.2
    size_t g_cnt = g_matrix_s.size();
    std::cout << "КОЛИЧЕСТВО ВЕРШИН: " << g_cnt << "\n";
    std::cout << "РАЗМЕР ГРАФА: " << e_cnt << "\n";
    print_matrix(g_matrix_s);   // 1.1
    count_s(g_matrix_s, neigh, loop);
    type_v(neigh, loop);   // 1.3

    //task2
    std::vector<std::vector<bool>> g_matrix_i = gen_matrix_i(g_cnt, e_cnt, g_matrix_s);   // 2.1
    print_matrix_i(g_matrix_i);   // 2.1
    std::cout << "\nРАЗМЕР ПО МАТРИЦЕ ИНЦИДЕНТНОСТИ:\n" << size_i(g_matrix_i) << "\n";   // 2.2
    count_i(g_matrix_i, neigh, loop);
    type_v(neigh, loop);   // 2.3
}

//кол во вершин задает пользователь,  порядок графа
// добвавить возможность формирования петель, корректное определение характеристик вершин