setwd("/home/itslab/Downloads/cfiles")

sum=0

los = read.csv("/home/itslab/Downloads/cfiles/Full_Data.csv")
	n=60	
	c1 = which(los$Class == 1)
	p1 = sample(c1,n)
	c2 = which(los$Class == 2)
	p2 = sample(c2,n)
	c3 = which(los$Class == 3)
	p3 = sample(c3,n)
	c4 = which(los$Class == 4)
	p4 = sample(c4,n)

for(i in 1:100)
{	
	i=1

	index = c(p1,p2,p3,p4)
	lostrain = los[-index,]
	lostest = los[index,]

	svmclass = svm(lostrain[,1:3],lostrain[,5],type="C",kernel = "radial",cost=100,gamma=0.01)
	svmoutput = predict(svmclass,lostest[,1:3]);
	#write.csv(knnclassification[1:314],paste("/home/itslab/Downloads/cfiles/outputknneditscale_k",i,".csv",sep=""))
	df = as.data.frame(cbind(lostest[,5],svmoutput[1:240]),stringsAsFactors = F)
	names(df) = c("actual","pred")
	print(c(i,length(which(df$actual == df$pred)),length(which(df$actual != df$pred))))
	write.csv(lostrain,"/home/itslab/Downloads/cfiles/besttraindatasvm12.csv")
	write.csv(lostest,"/home/itslab/Downloads/cfiles/besttestdatasvm12.csv")
	write.csv(svmoutput[1:240],"/home/itslab/Downloads/cfiles/bestclasssvm12.csv")
	
	sum = sum+length(which(df$actual != df$pred))
	}
print(sum/100)

