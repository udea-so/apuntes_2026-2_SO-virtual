# Problema productor consumidor

Todos estos problemas se basan en la implementación dada en el libro de Remzi que sigue la implementación del problema del productor consumidor [producer_consumer_works.c](https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/producer_consumer_works.c) del libro **Operating Systems: Three Easy Pieces** (de Remzi H. Arpaci-Dusseau and Andrea C. Arpaci-Dusseau). Cabe anotar que el código solo es valido asumiendo un solo productor. A continuación se listan los códigos adaptados donde se estudian gradualmente los ejemplos del libro:
1. Problema del productor consumidor con un unico buffer ([link](single_buffer/))
2. Problema del productor consumidor con buffer circular con race condition - **pc_attemp1** ([link](circular_buffer/pc_attemp1/))
3. Problema del productor consumidor con buffer circular con deadlock - **pc_attemp2** ([link](circular_buffer/pc_attemp2/))
4. Problema del productor consumidor con buffer circular donde se solucionan los problemas de race condition y deadlock - **pc_work** ([link](circular_buffer/pc_work/))

La implementación intacta del libro de Remzi de este problema con todo solucionado se encuentra en el siguiente [link](work/)

