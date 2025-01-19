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
'''
# Carregar o arquivo CSV em um DataFrame do pandas
dfPontos = pd.read_csv("pontos.csv", sep=";", header=0, encoding="iso-8859-1")
dfBairros = pd.read_csv("bairros.csv", sep=";", header=0, encoding="iso-8859-1")

# Listar os pontos de embarque da fkLinha = 55, somente os campos fkLinha, nmPonto
pontos_linha_55 = dfPontos[dfPontos['fkLinha'] == 55][['nmPonto']]
print(pontos_linha_55)

# Salvar o DataFrame pontos_linha_55 em um arquivo CSV
pontos_linha_55.to_csv("pontos_linha_55.csv", sep=";", index=False, encoding="iso-8859-1")
'''
pontos_linha_55 = pd.read_csv("pontos_linha_55.csv", sep=";", header=0, encoding="iso-8859-1")

# Adicionar o endereço da Nuclep ao dataset pontos_linha_55
nuclep_endereco = "NUCLEP"

# Criar um DataFrame com o endereço da Nuclep
df_nuclep = pd.DataFrame({'nmPonto': [nuclep_endereco]})

# Concatenar o DataFrame df_nuclep com pontos_linha_55
pontos_linha_55 = pd.concat([df_nuclep, pontos_linha_55], ignore_index=True)

# Reduzir o dataset pontos_linha_55 a somente os 5 primeiros registros
#pontos_linha_55 = pontos_linha_55.head(10)


print(pontos_linha_55)

# Loop para ler cada registro e atribuir valores de latitude e longitude
for index, row in pontos_linha_55.iterrows():
    coordenadas = retorne_coordenada(row['nmPonto'])
    print(row['nmPonto'])
    print(coordenadas)
    if coordenadas:
        pontos_linha_55.at[index, 'latitude'] = coordenadas[1]
        pontos_linha_55.at[index, 'longitude'] = coordenadas[0]
    else:
        pontos_linha_55.at[index, 'latitude'] = None
        pontos_linha_55.at[index, 'longitude'] = None

# Salvar o DataFrame atualizado em um arquivo CSV
pontos_linha_55.to_csv("pontos_linha_55_com_coordenadas.csv", sep=";", index=False, encoding="iso-8859-1")


#pontos_linha_55 = pd.read_csv("pontos_linha_55_com_coordenadas.csv", sep=";", header=0, encoding="iso-8859-1")

# Reduzir o dataset pontos_linha_55 a somente os 5 primeiros registros
#pontos_linha_55 = pontos_linha_55.head(3)

# Montar um array de coordenadas com o par latitude e longitude vindo do dataset pontos_linha_55
coordenadas = []
for index, row in pontos_linha_55.iterrows():
    if pd.notnull(row['longitude']) and pd.notnull(row['latitude']):
        coordenadas.append([row['longitude'], row['latitude']])


'''
# Exemplo de uso
coordenadas = [
    retorne_coordenada("Rua Mario Agostinelli, 155"),  # São Paulo
    retorne_coordenada("Rua Vlaminck, 64"),  # Rio de Janeiro
    retorne_coordenada("Rua Conde de Leopoldina, 480")    # Brasil
]
'''
print(coordenadas)

matriz_distancia = calcular_distancias(coordenadas)
print(matriz_distancia)
