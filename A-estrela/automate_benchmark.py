import subprocess
import re
import csv
import statistics
import os
from collections import defaultdict

# ============================================================
# CONFIGURAÇÃO
# ============================================================
EXECUTABLES = {
    "Dijkstra": "benchmark_dijkstra_final.exe",
    "A-Star": "benchmark_astar_final.exe",
    "Duan": "benchmark_duan_final.exe"
}

REPETITIONS = 10
OUTPUT_LOG = "benchmark_raw_output.log"
OUTPUT_CSV = "resultado_consolidado_benchmark.csv"

# Regex mais flexível:
# 1. DATASET: Procura por "PROCESSANDO:" e pega o que vem depois
DATASET_PATTERN = re.compile(r"PROCESSANDO:\s*(.+)", re.IGNORECASE)
# 2. TEMPO: Procura por "TEMPO", ignora o que tiver no meio, e pega o número (decimal)
TIME_PATTERN = re.compile(r"TEMPO.*[:\s]+([\d,.]+)", re.IGNORECASE)

def run_benchmarks():
    results = defaultdict(lambda: defaultdict(list))
    
    with open(OUTPUT_LOG, "w", encoding="utf-8") as log_file:
        print(f"Iniciando {REPETITIONS} execuções para cada algoritmo...\n")
        
        for algo_name, exe_path in EXECUTABLES.items():
            if not os.path.exists(exe_path):
                print(f"AVISO: {exe_path} não encontrado. Pulando...")
                continue
                
            print(f"Executando {algo_name}...", end=" ", flush=True)
            
            for i in range(REPETITIONS):
                try:
                    # Executa capturando stdout e stderr
                    # No Windows, 'cp1252' ou 'cp850' são mais seguros para capturar acentos do C
                    process = subprocess.run(
                        exe_path, # Removido o "./" para compatibilidade direta Windows
                        capture_output=True, 
                        text=True, 
                        encoding="latin-1", 
                        errors="replace",
                        shell=True
                    )
                    
                    output = process.stdout
                    log_file.write(f"--- {algo_name} Execução {i+1} ---\n{output}\n")
                    
                    current_dataset = None
                    lines = output.splitlines()
                    
                    found_in_this_run = 0
                    for line in lines:
                        # Identificar Dataset
                        ds_match = DATASET_PATTERN.search(line)
                        if ds_match:
                            current_dataset = ds_match.group(1).strip()
                        
                        # Identificar Tempo
                        t_match = TIME_PATTERN.search(line)
                        if t_match and current_dataset:
                            # Converte vírgula para ponto (padrão brasileiro vs americano)
                            raw_time = t_match.group(1).replace(',', '.')
                            try:
                                exec_time = float(raw_time)
                                results[algo_name][current_dataset].append(exec_time)
                                found_in_this_run += 1
                            except ValueError:
                                continue
                    
                    # Se não encontrou nada, imprime um aviso de debug para a primeira falha
                    if found_in_this_run == 0 and i == 0:
                        print(f"\n[DEBUG] {algo_name} rodou, mas não detectou padrões na saída:")
                        print(f"--- Início da Saída capturada ---\n{output[:200]}...\n--- Fim da Saída ---")

                except Exception as e:
                    print(f"\nErro fatal ao executar {exe_path}: {e}")
                    break
            
            print("OK!")

    return results

def save_to_csv(results):
    if not results:
        return

    all_datasets = set()
    for algo in results:
        for ds in results[algo]:
            all_datasets.add(ds)
    
    all_datasets = sorted(list(all_datasets))
    
    with open(OUTPUT_CSV, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["Algoritmo", "Dataset", "Tempo Médio (s)", "Desvio Padrão", "Amostras", "Min", "Max"])
        
        for algo in sorted(results.keys()):
            for ds in all_datasets:
                times = results[algo].get(ds, [])
                if times:
                    avg = statistics.mean(times)
                    stdev = statistics.stdev(times) if len(times) > 1 else 0
                    writer.writerow([
                        algo, ds, f"{avg:.6f}", f"{stdev:.6f}", 
                        len(times), f"{min(times):.6f}", f"{max(times):.6f}"
                    ])
                    
    print(f"\nPlanilha gerada: {OUTPUT_CSV}")
    print(f"Logs salvos: {OUTPUT_LOG}")

if __name__ == "__main__":
    res = run_benchmarks()
    if res:
        save_to_csv(res)
    else:
        print("\nERRO: Nenhum dado foi processado. Verifique os nomes dos datasets nos prints do C.")