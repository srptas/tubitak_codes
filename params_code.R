readparams(indexx, N , distruption_type, cpcr_, holdrate, ptilP_);
N<- c(5,10)
distruption_type<-c(0,1)
cpcr<-c(0.75,1,1.25)
holdrate<-c(0.1,0.2,0.25)
ptilP<-c(1,1.1,1.25,1.5)
expand.grid(N,distruption_type,cpcr,holdrate,ptilP)
a<-expand.grid(N,distruption_type,cpcr,holdrate,ptilP)

write.table(a,"params.txt",sep="\t",row.names = FALSE,col.names = FALSE)
