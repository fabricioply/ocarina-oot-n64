# Ocarina por microfone — N64 no PC

Mod experimental para **Ocarina of Time original de N64 no Ship of Harkinian 9.3.1, Windows x64**. Uma nota por vez, com instrumento, assobio ou cantarolar. Modo assistido por padrão: compara intervalos, permitindo transpor a frase inteira.

**Prévia experimental v0.1.4.** Os 11 grupos de testes passam. Foi corrigida a escuta bloqueada pela captura do mouse/teclado pelo ImGui mesmo com os menus fechados. A execução completa das músicas no jogo ainda precisa ser validada ao vivo após essa correção.

## Instalar

1. Prepare o [Ship of Harkinian 9.3.1 oficial](https://github.com/HarbourMasters/Shipwright/releases/tag/9.3.1) com seu dump compatível e confirme que o jogo funciona.
2. No jogo original, escolha **OpenGL** nas configurações gráficas e feche-o. Direct3D 11 apresentou falha nesta compilação.
3. Coloque a instalação oficial em `Jogo/`. Extraia o zip do mod na pasta acima de `Jogo/`, preservando a estrutura. Veja o exemplo no [README](README.md#install).
4. Execute `INICIAR_COM_MICROFONE.cmd`.
5. Selecione a entrada em **Esc > Settings > Audio > Microfone da Ocarina**. Feche os menus, equipe e saque a ocarina. Toque a frase completa, terminando com uma pausa.

O zip contém somente o executável modificado, iniciador e documentação/licenças. A instalação oficial e os arquivos `soh.o2r` e `oot.o2r` devem vir da sua própria instalação. Nenhuma ROM, asset extraído, save ou configuração pessoal é distribuída.

Faça backup: o mod compartilha `Jogo/Save` com o executável original.

## Usar

- Suporta as 12 músicas padrão, respeitando músicas aprendidas e condições normais do jogo.
- Uma nota por vez. Acordes e acompanhamento misturado não são suportados.
- Frases ambíguas abrem a caixa nativa de Sim/Não. A ou C-Up confirma; Não avança à próxima candidata; B cancela. Solte os botões para retomar.
- Notas repetidas precisam de articulação. O modo assistido tolera pequenas oscilações e enfeites.
- Notas manuais pelos botões cancelam a escuta até guardar e sacar a ocarina novamente.
- A saída de áudio fica muda durante a escuta para reduzir retorno, preservando o volume salvo.
- Não inclui espantalho personalizado, ensino/gravação ou minijogos de ocarina.

O processamento é local, sem gravação de áudio. Os logs contêm medições de níveis/notas e podem conter identificadores do dispositivo; revise antes de compartilhar.

Esta versão é para o jogo N64 no PC, não para OoT 3D nem para console N64 físico. Instruções de compilação: [BUILDING.md](docs/BUILDING.md). Créditos e licenças: [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
