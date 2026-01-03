import csv
import random
import time

# ==========================================
# CONFIGURAÇÃO DO GRAFO MASSIVO
# ==========================================
# 1 milhão de nós (ajuste conforme sua RAM, 10^7 seria o ideal para o Duan)
NUM_NODES = 1000000 
NUM_SHORTCUTS = 2000000 # m ~= 3n (Grafo esparso)
FILENAME_EDGES = "massivo_edges.csv"
FILENAME_NODES = "massivo_nodes.csv"

def generate_massive_dataset():
    print(f"--- Gerando Grafo Massivo: {NUM_NODES} nós ---")
    start_time = time.time()

    # 1. Gerar Nós (Linearmente para facilitar a busca de pior caso)
    print("Salvando Nós...")
    with open(FILENAME_NODES, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['id', 'osmid', 'x', 'y'])
        for i in range(NUM_NODES):
            # Posicionamos os nós em uma linha reta
            writer.writerow([i, 0, i, 0])

    # 2. Gerar Arestas
    print("Gerando Arestas (Espinha + Atalhos)...")
    with open(FILENAME_EDGES, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['source', 'target', 'weight'])
        
        # A: Espinha Dorsal (Garante que o caminho mínimo seja longo)
        for i in range(NUM_NODES - 1):
            # Pesos de ponto flutuante para desafiar a ordenação
            weight = random.uniform(10.0, 11.0)
            writer.writerow([i, i + 1, int(weight * 100)]) # Convertido para int para seu código C atual

        # B: Atalhos Aleatórios (Força o decreaseKey no Dijkstra)
        for _ in range(NUM_SHORTCUTS):
            u = random.randint(0, NUM_NODES - 2)
            # Atalho pula entre 2 e 100 nós
            jump = random.randint(2, 100)
            v = min(u + jump, NUM_NODES - 1)
            
            if u != v:
                # Atalho com peso ligeiramente menor para ser atrativo
                weight = random.uniform(5.0, 15.0) * jump
                writer.writerow([u, v, int(weight * 10)])

    end_time = time.time()
    print(f"\nSUCESSO!")
    print(f"Tempo de geração: {end_time - start_time:.2f}s")
    print(f"Arquivos: {FILENAME_NODES} e {FILENAME_EDGES}")
    print(f"Tamanho aproximado: {NUM_NODES} nós e {NUM_NODES + NUM_SHORTCUTS} arestas.")
    print(f"DICA: No seu código C, aumente NODE_COUNT_ESTIMATE para {NUM_NODES + 100}.")

if __name__ == "__main__":
    generate_massive_dataset()