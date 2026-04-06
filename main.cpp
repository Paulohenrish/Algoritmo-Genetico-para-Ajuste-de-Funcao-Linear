#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <limits>
#include <iomanip>

struct Point {
    double x;
    double y;
};

struct Individual {
    double a;
    double b;
};

void carregarArquivoEntrada(const std::string& nome_arquivo, int& n, int& m, int& G, std::vector<Point>& conjunto_dados) {
    std::ifstream file(nome_arquivo);
    if (!file.is_open()) {
        std::cerr << "Falha na leitura: Arquivo " << nome_arquivo << " ausente." << std::endl;
        exit(EXIT_FAILURE);
    }

    file >> n >> m >> G;
    conjunto_dados.resize(n); 
    
    for (int i = 0; i < n; ++i) {
        file >> conjunto_dados[i].x >> conjunto_dados[i].y; 
    }
    file.close();
}

void alocarEDistribuirPopulacao(std::vector<Individual>& pop, int m, std::mt19937& motor_estocastico) {
    std::uniform_real_distribution<double> faixas_aleatorias(-10.0, 10.0);
    for (int i = 0; i < m; ++i) {
        pop[i].a = faixas_aleatorias(motor_estocastico); 
        pop[i].b = faixas_aleatorias(motor_estocastico); 
    }
}

void julgarAptidaoDemografica(const std::vector<Individual>& pop, std::vector<double>& indices_fitness, std::vector<double>& registro_erros, int m, int n, const std::vector<Point>& conjunto_dados) {
    for (int i = 0; i < m; ++i) {
        double parametro_a = pop[i].a;
        double parametro_b = pop[i].b;
        double acumulo_mse = 0.0;

        for (int j = 0; j < n; ++j) {
            double previsional_y = (parametro_a * conjunto_dados[j].x) + parametro_b;
            double diferenca_bruta = conjunto_dados[j].y - previsional_y; 
            acumulo_mse += (diferenca_bruta * diferenca_bruta);
        }
        acumulo_mse /= n; 

        registro_erros[i] = acumulo_mse;
        indices_fitness[i] = 1.0 / (1.0 + acumulo_mse); 
    }
}

void identificarSuperioresHibridizacao(const std::vector<double>& indices_fitness, int m, int& pai_1_idx, int& pai_2_idx) {
    double base_max_primaria = -1.0;
    double base_max_secundaria = -1.0;
    pai_1_idx = 0;
    pai_2_idx = 0;

    for (int i = 0; i < m; ++i) {
        if (indices_fitness[i] > base_max_primaria) {
            base_max_secundaria = base_max_primaria;
            pai_2_idx = pai_1_idx;
            base_max_primaria = indices_fitness[i];
            pai_1_idx = i;
        } else if (indices_fitness[i] > base_max_secundaria) {
            base_max_secundaria = indices_fitness[i];
            pai_2_idx = i;
        }
    }
}

int localizarEspecimeEstagnado(const std::vector<double>& indices_fitness, int m) {
    double limite_inferior_extremo = std::numeric_limits<double>::max();
    int chave_indice_morte = -1;

    for (int i = 0; i < m; ++i) {
        if (indices_fitness[i] < limite_inferior_extremo) {
            limite_inferior_extremo = indices_fitness[i];
            chave_indice_morte = i;
        }
    }
    return chave_indice_morte;
}

Individual executarPareamentoCrossover(const std::vector<Individual>& pop, int pai_1_idx, int pai_2_idx, std::mt19937& motor_estocastico) {
    std::uniform_real_distribution<double> chance(0.0, 1.0);
    Individual prole;
    
    if (chance(motor_estocastico) < 0.5) {
        prole.a = pop[pai_1_idx].a;
        prole.b = pop[pai_2_idx].b;
    } else {
        prole.a = pop[pai_2_idx].a;
        prole.b = pop[pai_1_idx].b;
    }
    return prole;
}

void induzirPerturbacaoMutacional(Individual& prole, double taxa_de_chance, double espessura_delta, std::mt19937& motor_estocastico) {
    std::uniform_real_distribution<double> medidor_chance(0.0, 1.0);
    std::uniform_real_distribution<double> alteracao_ambiental(-espessura_delta, espessura_delta);

    if (medidor_chance(motor_estocastico) < taxa_de_chance) {
        if (medidor_chance(motor_estocastico) < 0.5) {
            prole.a += alteracao_ambiental(motor_estocastico);
        } else {
            prole.b += alteracao_ambiental(motor_estocastico);
        }
    }
}

void consolidarRelatorioGeracional(std::ofstream& fluxo_saida, int iteracao, double fitness_apice, double erro_base, double a_vencedor, double b_vencedor) {
    fluxo_saida << "Evolucao " << std::setw(5) << iteracao 
        << " | Registro Apice Aptidao: " << std::fixed << std::setprecision(8) << fitness_apice 
        << " | Residual MSE: " << erro_base 
        << " | Var A: " << a_vencedor 
        << " | Var B: " << b_vencedor << "\n";
}

int main() {
    int carga_n = 0, carga_m = 0, eras_G = 0;
    
    std::vector<Point> espelho_amostral;

    carregarArquivoEntrada("input.dat", carga_n, carga_m, eras_G, espelho_amostral);

    std::vector<Individual> populacao_genoma(carga_m);
    std::vector<double> log_fitness_matrix(carga_m);
    std::vector<double> log_errors_matrix(carga_m);

    std::random_device gerador_entropico;
    std::mt19937 motor_estocastico(gerador_entropico());

    alocarEDistribuirPopulacao(populacao_genoma, carga_m, motor_estocastico);

    std::ofstream handler_saida("output.dat");
    if (!handler_saida.is_open()) {
        std::cerr << "Falha ao gravar output.dat" << std::endl;
        return EXIT_FAILURE;
    }

    const double BARREIRA_TAXA_MUTACIONAL = 0.85; 
    const double DISTURBIO_MAX_DELTA = 1.50; 

    for (int etapa = 1; etapa <= eras_G; ++etapa) {
        julgarAptidaoDemografica(populacao_genoma, log_fitness_matrix, log_errors_matrix, carga_m, carga_n, espelho_amostral);

        int ch_absoluta_idx = 0;
        double ch_topo_geracao = -1.0; 
        
        for(int t = 0; t < carga_m; ++t) {
            if(log_fitness_matrix[t] > ch_topo_geracao) {
                ch_topo_geracao = log_fitness_matrix[t];
                ch_absoluta_idx = t;
            }
        }

        consolidarRelatorioGeracional(handler_saida, etapa, log_fitness_matrix[ch_absoluta_idx], log_errors_matrix[ch_absoluta_idx], populacao_genoma[ch_absoluta_idx].a, populacao_genoma[ch_absoluta_idx].b);

        int reprodutor_primario, reprodutor_secundario;
        identificarSuperioresHibridizacao(log_fitness_matrix, carga_m, reprodutor_primario, reprodutor_secundario);

        Individual descendencia_nova = executarPareamentoCrossover(populacao_genoma, reprodutor_primario, reprodutor_secundario, motor_estocastico);

        induzirPerturbacaoMutacional(descendencia_nova, BARREIRA_TAXA_MUTACIONAL, DISTURBIO_MAX_DELTA, motor_estocastico);

        int varredura_pior_especime = localizarEspecimeEstagnado(log_fitness_matrix, carga_m);
        
        populacao_genoma[varredura_pior_especime] = descendencia_nova; 
    }

    handler_saida.close();
    

    return EXIT_SUCCESS;
}