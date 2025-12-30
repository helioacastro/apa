import csv
import os

# ==========================================
# CONFIGURAÇÃO
# ==========================================
# Nome do arquivo que você baixou e salvou na pasta

input_filename = {"brc202d", "ost000a"}
output_prefix = {"game_map_brc202d", "game_map_ost000a"}

#INPUT_FILENAME = "brc202d.map"
#OUTPUT_PREFIX = "game_map_brc202d"

def generate_dataset_from_local_file(INPUT_FILENAME, OUTPUT_PREFIX):

    print(f"--- Processando Mapa Local: {INPUT_FILENAME} ---")
    
    # 1. Leitura do Arquivo Local
    if not os.path.exists(INPUT_FILENAME):
        print(f"ERRO CRÍTICO: Arquivo '{INPUT_FILENAME}' não encontrado.")
        print("Por favor, baixe o mapa em 'https://movingai.com/benchmarks/dao/brc202d.map'")
        print("e salve-o na mesma pasta deste script.")
        return

    try:
        with open(INPUT_FILENAME, 'r') as f:
            content = f.read().splitlines()
    except Exception as e:
        print(f"Erro ao ler arquivo: {e}")
        return

    # 2. Parse do Cabeçalho
    height = 0
    width = 0
    header_end_index = 0

    # Varre as primeiras linhas procurando os metadados
    for i, line in enumerate(content[:10]): # Olha só as 10 primeiras linhas
        parts = line.strip().split()
        if not parts: continue
        
        if parts[0] == "height":
            height = int(parts[1])
        elif parts[0] == "width":
            width = int(parts[1])
        elif parts[0] == "map":
            header_end_index = i + 1
            break
            
    if height == 0 or width == 0:
        print("Erro: Formato de mapa inválido (height/width não encontrados).")
        return

    print(f"Dimensões detectadas: {width} x {height}")

    # 3. Processamento do Grid
    print("Convertendo grid para grafo...")
    grid = content[header_end_index:]
    
    passable_chars = {'.', 'G', 'S', 'T'}
    node_mapping = {} # (x,y) -> ID
    nodes_list = []
    
    current_id = 0
    
    # Passo A: Identificar Nós
    for y in range(len(grid)):
        line = grid[y]
        for x in range(len(line)):
            char = line[x]
            if char in passable_chars:
                node_mapping[(x, y)] = current_id
                nodes_list.append([current_id, x, y]) 
                current_id += 1

    print(f"Nós transitáveis: {len(nodes_list)}")

    if len(nodes_list) == 0:
        print("Erro: Mapa vazio ou sem áreas transitáveis.")
        return

    # Passo B: Criar Arestas (8-vizinhos)
    edges_list = []
    moves = [
        (0, 1, 10),  (0, -1, 10), (1, 0, 10),  (-1, 0, 10),  # Reto = 10
        (1, 1, 14),  (1, -1, 14), (-1, 1, 14), (-1, -1, 14)  # Diagonal = 14
    ]
    
    for x, y in node_mapping.keys():
        u_id = node_mapping[(x, y)]
        
        for dx, dy, cost in moves:
            nx, ny = x + dx, y + dy
            if (nx, ny) in node_mapping:
                v_id = node_mapping[(nx, ny)]
                edges_list.append([u_id, v_id, cost])

    print(f"Arestas geradas: {len(edges_list)}")

    # 4. Exportar
    print("Salvando CSVs...")
    
    # Nós
    with open(f'{OUTPUT_PREFIX}_nodes.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['id', 'osmid_fake', 'x', 'y'])
        for node in nodes_list:
            writer.writerow([node[0], 0, node[1], node[2]])

    # Arestas
    with open(f'{OUTPUT_PREFIX}_edges.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['source', 'target', 'weight'])
        for edge in edges_list:
            writer.writerow(edge)

    # Pegar extremos para teste
    id_origem = nodes_list[0][0]
    id_destino = nodes_list[-1][0]

    print("\n" + "="*50)
    print("SUCESSO!")
    print(f"Grafo gerado em: '{OUTPUT_PREFIX}_edges.csv'")
    print(f"Origem sugerida: {id_origem}")
    print(f"Destino sugerido: {id_destino}")
    print("="*50)

if __name__ == "__main__":
    for filename in input_filename:
        generate_dataset_from_local_file(filename + ".map", filename)