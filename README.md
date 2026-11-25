# SISBIB: Sistema de Gestão de Biblioteca

Sistema de biblioteca em linguagem C, executado no terminal, para controlar usuários, autores, livros, estoque e empréstimos. Os dados ficam salvos em arquivos CSV, sem precisar de banco de dados.

## Funcionalidades

- **Usuários:** cadastrar, listar, atualizar e excluir. Cada usuário tem um status: `ATIVO` (padrão), `INATIVO` ou `SUSPENSO`.
- **Autores:** cadastrar, listar, atualizar e excluir.
- **Livros:** cadastrar, listar, atualizar e excluir, com até 3 autores por livro. Se o autor ainda não existe, dá para cadastrá-lo na hora.
- **Estoque:** cada livro guarda a quantidade de exemplares, e a listagem mostra o total e quantos estão disponíveis.
- **Empréstimos:** registro, alteração e relatório de empréstimos, com status `PENDENTE` ou `DEVOLVIDO`.
- **IDs automáticos:** o sistema gera o próximo ID sozinho, a partir de 100.
- **Regras de integridade:**
  - não permite excluir um autor que esteja vinculado a um livro;
  - não permite excluir um livro que tenha empréstimo pendente.
- **Arquivos criados automaticamente:** se os CSV não existirem, o programa cria cada um com o cabeçalho.

## Tecnologias

- Linguagem C
- Bibliotecas padrão: `stdio.h`, `string.h` e `unistd.h`
- Armazenamento em arquivos CSV (separador `;`)

## Estrutura do projeto

| Arquivo | Descrição |
| --- | --- |
| `SISBIB.C` | Código-fonte do sistema |
| `SISBIB.exe` | Executável para Windows |
| `usuario.csv` | Dados dos usuários |
| `autor.csv` | Dados dos autores |
| `livros.csv` | Dados dos livros e do estoque |
| `emprestimo.csv` | Dados dos empréstimos |

## Formato dos dados

Cada arquivo CSV começa com uma linha de cabeçalho.

```text
usuario.csv     id;nome;status
autor.csv       id;nome
livros.csv      id;titulo;idautores;estoque     (autores no formato 101|102|0)
emprestimo.csv  id;id_usuario;id_livro;status
```

## Como executar

### Windows

Execute o arquivo `SISBIB.exe` na mesma pasta onde estão os arquivos `.csv`.

> **Importante:** o programa lê e grava os arquivos `.csv` na pasta em que é executado. Rode sempre a partir da pasta do projeto.

## Como usar

O sistema funciona pelo terminal: você digita o número da opção desejada e confirma com Enter. A tela é limpa entre as ações, e o programa avisa quando uma opção é inválida.

## O que pratiquei neste projeto

- Uso de `struct` para representar usuários, livros, autores e empréstimos
- Leitura e escrita de arquivos (`fopen`, `fgets`, `fscanf`, `fprintf`)
- Atualização e exclusão de registros com arquivo temporário
- Validação de entradas do usuário
- Separação do código em funções

## Autor

**Victor Hugo**
GitHub: [@Correia7hs](https://github.com/Correia7hs)
