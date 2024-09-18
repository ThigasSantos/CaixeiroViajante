import matplotlib.pyplot as plt

# Função para gerar o gráfico
def plot_graph(x_values, y_values):
    y_values_in_minutes = [y / (60 * 1e6) for y in y_values]

    plt.figure(figsize=(8, 6))
    plt.plot(x_values, y_values_in_minutes, marker='o', linestyle='-', color='b', label='TSPD')
    plt.title('Threads x Tempo')
    plt.xlabel('Quantidade de Threads')
    plt.ylabel('Tempo em Minutos')
    plt.grid(True)
    plt.legend()
    plt.show()

# Exemplo de uso
# x = [1, 2, 3, 4, 5, 6, 8, 10, 15]
# y = [1690169622, 100988106, 70104311, 73931601, 38012616, 38328047, 37602236, 37443089, 38572233]

x1 = [1,2,3,4,5,6,8,10,15,25]
y = [1690169622, 100988106, 70104311, 73931601, 38012616,38328047,37602236,37443089,38572233, 37635453]

# Gerar o gráfico
plot_graph(x1, y)


import matplotlib.pyplot as plt

# Função para gerar o gráfico com duas linhas
def plot_two_lines_with_annotation(x_values, y_values1, y_values2):
    # Convertendo ambos os vetores de Y para minutos
    y_values1_in_minutes = [y / (60 * 1e6) for y in y_values1]
    y_values2_in_minutes = [y / (60 * 1e6) for y in y_values2]
    
    plt.figure(figsize=(8, 6))
    
    # Linha 1: Y1 em minutos
    plt.plot(x_values[:-1], y_values1_in_minutes[:-1], marker='o', linestyle='-', color='b', label='Sequencial (minutos)')
    
    # Linha 2: Y2 em minutos
    plt.plot(x_values, y_values2_in_minutes, marker='o', linestyle='-', color='r', label='Paralelo (minutos)')
    
    plt.title('Quantidade de Pontos X Tempo (em minutos)')
    plt.xlabel('Quantidade de Pontos')
    plt.ylabel('Tempo (minutos)')
    plt.grid(True)
    plt.legend()
    plt.show()

# Exemplo de uso
x = [6, 8, 10, 12, 14, 16]  # Valores de X são os mesmos
y1 = [5841150, 4750785, 31308823, 18449512, 1639734738, 10000000]  # Valores de Y1 em microssegundos (último será >300 minutos)
y2 = [5128693, 6349206, 12979795, 13915669, 38731350, 6581379845]  # Valores de Y2 em microssegundos

# Gerar o gráfico com duas linhas (Y1 e Y2), com último valor de Y1 indeterminado
plot_two_lines_with_annotation(x, y1, y2)



import pandas as pd

# Função para criar a tabela
def create_table(x_values, y_values):
    # Convertendo microssegundos para minutos
    y_values_in_minutes = [y / (60 * 1e6) for y in y_values]
    
    # Criando um DataFrame com os valores
    df = pd.DataFrame({
        'X': x_values,
        'Y (microssegundos)': y_values,
        'Y (minutos)': y_values_in_minutes
    })
    
    return df

# Criar a tabela
tabela = create_table(x1, y)

# Exibir a tabela
print(tabela)