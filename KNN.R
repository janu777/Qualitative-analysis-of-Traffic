
setwd("/home/itslab/Downloads/cfiles")
for(i in 1:300)
{	lostrain = read.csv("/home/itslab/Downloads/cfiles/LoS_TrainEdit.csv");
	lostest = read.csv("/home/itslab/Downloads/cfiles/LoS_TestEdit.csv");
	knnclassification = knn(lostrain[,7:10],lostest[,7:10],lostrain[,5],k=i,algorithm = "kd_tree");
	write.csv(knnclassification[1:314],paste("/home/itslab/Downloads/cfiles/outputknneditscale_k",i,".csv",sep=""))
	df = as.data.frame(cbind(lostest[,5],knnclassification[1:314]),stringsAsFactors = F)
	names(df) = c("actual","pred")
	print(c(i,length(which(df$actual == df$pred)),length(which(df$actual != df$pred))))
}
#for(j in 1:50)
#{
#sum=0

#for(i in 1:100)
#{	
	i=1


	los = read.csv("Full_Data.csv")
	n=60	
	c1 = which(los$Class == 1)
	p1 = sample(c1,n)
	c2 = which(los$Class == 2)
	p2 = sample(c2,n)
	c3 = which(los$Class == 3)
	p3 = sample(c3,n)
	c4 = which(los$Class == 4)
	p4 = sample(c4,n)


	index = c(p1,p2,p3,p4)
	lostrain = los[-index,]
	lostest = los[index,]

	knnclassification = knn(lostrain[,1:3],lostest[,1:3],lostrain[,5],k=8,algorithm = "kd_tree");
	write.csv(knnclassification[1:314],paste("/home/itslab/Downloads/cfiles/outputknneditscale_k",i,".csv",sep=""))
	df = as.data.frame(cbind(lostest[,5],knnclassification[1:240]),stringsAsFactors = F)
	names(df) = c("actual","pred")
	print(c(i,length(which(df$actual == df$pred)),length(which(df$actual != df$pred))))
	write.csv(lostrain,"besttraindataknn15.csv")
	write.csv(lostest,"besttestdataknn15.csv")
	write.csv(df,"bestclassknn15.csv")
	#sum = sum+length(which(df$actual != df$pred))
	#}
print(c(j,sum/100))
}

