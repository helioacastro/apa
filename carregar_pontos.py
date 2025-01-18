import pandas as pd
import requests

def retorne_coordenada(endereco):
    api_key = 'cf30db6de8bf4244b1626a7688b31701'
    url = f'https://api.geoapify.com/v1/geocode/search?text={endereco}&apiKey={api_key}'
    response = requests.get(url)
    if response.status_code == 200:
        data = response.json()
        if data['features']:
            coordinates = data['features'][0]['geometry']['coordinates']
            return coordinates
    return None

def calcular_distancias(coordenadas):
    api_key = 'cf30db6de8bf4244b1626a7688b31701'
    url = 'https://api.geoapify.com/v1/routematrix?apiKey=' + api_key
    payload = {
        "mode": "drive",
        "sources": [{"location": coord} for coord in coordenadas],
        "targets": [{"location": coord} for coord in coordenadas]
    }
    response = requests.post(url, json=payload)
    if response.status_code == 200:
        data = response.json()
        if 'sources_to_targets' in data:
            distancias = []
            for linha in data['sources_to_targets']:
                distancias.append([item['distance'] for item in linha])
            return distancias
    return None

# Carregar o arquivo CSV em um DataFrame do pandas
dfPontos = pd.read_csv("pontos.csv", sep=";", header=0, encoding="iso-8859-1")

# Listar os pontos de embarque da fkLinha = 55, somente os campos fkLinha, nmPonto
pontos_linha_55 = dfPontos[dfPontos['fkLinha'] == 55][['nmPonto']]
print(pontos_linha_55)

# Salvar o DataFrame pontos_linha_55 em um arquivo CSV
pontos_linha_55.to_csv("pontos_linha_55.csv", sep=";", index=False, encoding="iso-8859-1")

# Reduzir o dataset pontos_linha_55 a somente os 5 primeiros registros
pontos_linha_55 = pontos_linha_55.head(5)

# Adicionar colunas de latitude e longitude ao DataFrame pontos_linha_55
#pontos_linha_55['latitude'] = pontos_linha_55['nmPonto'].apply(lambda x: retorne_coordenada(x)[1] if retorne_coordenada(x) else None)
#pontos_linha_55['longitude'] = pontos_linha_55['nmPonto'].apply(lambda x: retorne_coordenada(x)[0] if retorne_coordenada(x) else None)

print(pontos_linha_55)

# Exemplo de uso
coordenadas = [
    retorne_coordenada("Rua Mario Agostinelli, 155"),  # São Paulo
    retorne_coordenada("Rua Vlaminck, 64"),  # Rio de Janeiro
    retorne_coordenada("Rua Conde de Leopoldina, 480")    # Brasil
]
matriz_distancia = calcular_distancias(coordenadas)
print(matriz_distancia)

