# Introdução

Este projeto foi desenvolvido no sexto semestre na disciplina de Sistemas Operacionais B.
O seu objetivo foi criar duas chamadas de sistema, uma chamada de sistema que criptografa dados
e outra que descriptografa os dados. Ambas utilizam o algoritmo ecb no modo aes.

# Começando a utilizá-lo

Este projeto deve ser utilizado em um sistema operacional Linux.
Primeiramente após realizar o download dos arquivos deve-se utilizar o comando make
na pasta onde os arquivos se encontram.
Estes arquivos devem ser colocados no diretório:
"/home/Kernel/linux-version/read_write_crypt".
Deve ser acessado o arquivo "syscall_64.tbl" na pasta
"/home/Kernel/linux-version/arch/x86/entry/syscalls/" e adicionar os próximos valores
reservados para chamadas de x64 com os seguintes parâmetros, além de seu valor, 
"common  read_crypt       read_crypt" e "common  write_crypt       write_crypt".
Após isto, deve-se acrescentar a pasta "read_write_crypt" no Makefile do Kernel do Linux.
Para finalizar deve-se alterar o arquivo "syscalls.h" na pasta
"/home/Kernel/linux-version/include/linux/", neste arquivo deve-se adicionar as linhas:
"asmlinkage ssize_t write_crypt(int fd, const void *buf, size_t nbytes);
 asmlinkage ssize_t read_crypt(int fd, void *buf, size_t nbytes);".
O último passo seria recompilar o Kernel do Linux e utilizar o arquivo de teste para utilizar estas
chamas de sistema.
