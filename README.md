# VibrationSensorProject
### Projeto inicial criação de um sensor de vibração utilizando placa Esp32 Heltec LoRa com integração de um sistema supervisório rodando em uma página web com tecnolgias de atualização automática e em tempo real.
# Descrição geral do Projeto:
O projeto tem por objetivo a criação de um sensor de vibração para o monitoramento de motores. Nesse viés, uma placa PCB foi projetada em conjunto com um software para controle da mesma. De modo geral, serão utilizados microcontroladores ESP32-Heltec_LoRa e sensores NEMA do tipo MPU925x: acelerômetro, giroscópio, magnetômetro e temperatura. O projeto é baseado no padrão Master-Slave onde, o slave é o dispositivo que estará em campo coletando os dados pelo sensor e enviando para o master através da comunicação LoRa em banda de transmissão específica. Já o dispositivo Master, receberá o pacote de dados de envio contínuo, ou seja, em tempo real, transformará em arquivo JSON e nele irá gerar um servidor para que os dados possam ser amostrados através de uma página web utilizando javascript, html e css. 
No entanto, antes do monitoramento faz-se necessário configurações iniciais como, calibração do sensores, leitura de espectro normal para que assim o sistema possa supervisionar os dados e comparar com os dados normais lidos. A figura a seguir elucida o primeiro protótipo do dispositivo medidor de vibração(objeto da direita) em conjunto com o disposivo receptor(WebServer) na parte esquerda.


![WhatsApp Image 2022-08-26 at 19 35 10](https://user-images.githubusercontent.com/35776840/187000401-ed612b72-6ba7-4e61-a31b-bb84867efc3c.jpeg)


## Slave
- O dispositivo slave que ficará no campo, assim que iniciado deverá executar algumas configurações iniciais, sendo elas: digitar o id  do qeuipamento a qual está sendo plugado, calibrar os sensores, informar um tempo para leitura do padrão de normalidade. No entanto para desbloquear essas opções, é necessário inserir uma senha que no caso seria apertar os dois botões presentes na PCB por 2 segundos. 
![senha](https://user-images.githubusercontent.com/35776840/185218320-6a20858e-8171-415a-bfb5-88dbb0a2a4e2.jpg)

- Após desbloquear o settings do sensor, a próxima tela é a de informar  o id do equipamento a qual o dispositivo está sensdo plugada Ex; (S7-INJR-002), através do teclado de dois botões. O primeiro botão é responsável por navegar por todos os caracteres e números já o segundo botão seleciona o caractere escolhido e passa para o próximo índice.
![id](https://user-images.githubusercontent.com/35776840/185218334-7e985e14-4d31-4198-909f-d39aaf8238d3.jpg)

- Ao escolher o id do equipamento, o róximo estágio é a calibração do sensor, devido à alta taxa de leitura os sensores levam um tempo até ajustar suas leituras, essa etapa demora alguns segundos e é informada na tela.
![calibração](https://user-images.githubusercontent.com/35776840/185218357-d7bb144c-2f7d-46b5-9a00-63255de5b48f.jpg)

-Após o sensor está calibrado, é necessário escolher um tempo de leitura do padrão de normalidade. Nessa leitura deve ocorrer no equipamento funcionando no seu aspecto normal, pois os dados lidos nesta etapa serão salvos e armazenados para comparação posterior 
![tempo1](https://user-images.githubusercontent.com/35776840/185218379-40d4ca92-f465-4353-923b-072479b4324b.jpg)
![padrão](https://user-images.githubusercontent.com/35776840/185218370-dcdc74dd-3cf6-426f-9c99-61a0a2d329e1.jpg)

## Master
- O dispostivo Master, é somente uma placa ESP32, com comunicação wifi e antena para recepção do sinal LoRa enviado do Slave. Nesse dispositivo o mesmo é transformado em um ponto de acesso e um webserver, carregando uma página html que no back-end recebe os dados em formato Json e são mostrados de forma gráfica e alertas no Front-end utilizando JavaScript. Para que a página seja carregada, o computador e o Master devem estar conectados na mesma rede wifi. Além disso, no display do dispositivo master, os dados dos três eixos de aceleração são exibidos, em conjunto com a potência do sinal de transmissão. A Figura a seguir, elucida o hardware do dispositivo Master.


![WhatsApp Image 2022-08-26 at 19 35 45](https://user-images.githubusercontent.com/35776840/187000453-6d3f2acb-95b8-4c18-b9e2-50a4e51a55b5.jpeg)



Uma vez estabelecida as conexões, uma página supervisória é gerada e pode ser visualizada a seguir:


### Sistema funcionando em normalidade
![fig supervs 1](https://user-images.githubusercontent.com/35776840/186803239-dcea84ae-0fc1-41f5-be7a-39202db6a05c.png)


### Sistema funcionando em condições de distúrbio
![fig supervis 2](https://user-images.githubusercontent.com/35776840/186803243-2c1b4b12-a419-4a1f-8dfc-ce21099697f5.png)

# Melhorias
Melhorias podem ser feitas, como:
- Adicionar lisview para selecionar qual máquina deseja-se ver o monitoramento
- Enviar alertas via e-mail ou sms para o setor responsável
- Analisar os dados no espectro da frequência utilizando Transformada de Fourier para uma melhor análise de preditiva da máquina.
- Salvar os dados de diagnóstico para montar um banco de dados e treinar uma classificador de Machine Learning para integrar no sistema supervisório indicativos de possíveis fallhas, dentre outras ideias


@By; Manoel Eric - Estagiário de Automação industrial, supervisionado por Valmir Mesquita(Setor Novas Tecnologias - NovTec)
