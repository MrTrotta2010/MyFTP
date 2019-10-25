# Trabalhos-UFSJ

Repositório com os trabalhos da facu

Para instalar:
- sudo apt-install git

Para clonar o repositório:
- git clone <link no github> //Na pasta criada

Para criar uma branch:
- git checkout -b <nome da branch>
    ou
- git branch <nome da branch>
  
Para atualizar a branch: 
- git add <nome do arquivo> //Adiciona os arquivos à lista de arquivos
- git commit -m "Descrição da atualização"
- git push origin <nome da branch>

Para atualizar o repositório local:
- git pull <link no github> <nome da branch>
  
Para juntar as branches A e B:
- git checkout A //Entrar em uma das branches
- git pull <link no github> B
- git commit -m "Descrição" //Dar commit no merge
- Revisar as mudanças e/ou resolver os conflitos
- git push origin A //Empurrar a branch para o github
