"""
读取数据部分
"""
import torch
from torch import nn
from torch.utils.data import DataLoader
from torchvision import datasets as ds
import torchvision.transforms as transforms
transform = transforms.Compose([transforms.ToTensor(),
                                transforms.Normalize(mean=(0.5, 0.5, 0.5), std=(0.5, 0.5, 0.5))])

train_set = ds.CIFAR10('cifar10_python', train=True, transform=transform)
test_set = ds.CIFAR10('cifar10_python', train=True, transform=transform)

train_loader = DataLoader(train_set, batch_size=10, shuffle=True)
test_loader = DataLoader(test_set, batch_size=10, shuffle=True)
# 设置各类别值对应的类别名
classes_dict = {0: 'plane', 1: 'car', 2: 'bird', 3: 'cat', 4: 'deer',
                5: 'dog', 6: 'frog', 7: 'horse', 8: 'ship', 9: 'truck'}
 
"""
LeNet-5网络
"""


class LeNet(nn.Module):
    def __init__(self):
        # 初始化父类nn.Module
        super(LeNet, self).__init__()
        # 第一层卷积层（包含非线性变化和池化），使用nn.Sequential进行封装
        self.conv_1 = nn.Sequential(
            nn.Conv2d(in_channels=3, out_channels=6, kernel_size=5, stride=1),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2)
        )
        # 第二层卷积层（包含非线性变化和池化），使用nn.Sequential进行封装
        self.conv_2 = nn.Sequential(
            nn.Conv2d(in_channels=6, out_channels=16, kernel_size=5, stride=1),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2)
        )
        # 第三层卷积层（包含非线性变化和池化），使用nn.Sequential进行封装
        self.conv_3 = nn.Sequential(
            nn.Conv2d(in_channels=16, out_channels=120,
                      kernel_size=5, stride=1),
            nn.ReLU()
        )
        # 第一层全连接层
        self.fc_1 = nn.Sequential(
            nn.Linear(120, 84),
            nn.ReLU()
        )
        # 第二层全连接层
        self.fc_2 = nn.Linear(84, 10)

    def forward(self, x):
        x_conv1 = self.conv_1(x)
        x_conv2 = self.conv_2(x_conv1)
        x_conv3 = self.conv_3(x_conv2)

        x_conv3 = x_conv3.view(x.size()[0], -1)

        x_fc1 = self.fc_1(x_conv3)
        x_fc2 = self.fc_2(x_fc1)
        return x_fc2



"""
网络训练部分
"""
# 网络：实例化一个LeNet网络
net = LeNet()
# 损失函数：网络损失函数定义为交叉熵函数
Loss_function = nn.CrossEntropyLoss()
# 优化方法
optimizer = torch.optim.SGD(net.parameters(), lr=0.001, momentum=0.9)

# epoch代表遍历完所有样本的过程，将epoch设置20，即遍历完样本20次
for epoch in range(20):
    # 调用之前的train_loader迭代器
    for i, data in enumerate(train_loader):
        # inputs代表图像，labels代表对应的标签
        inputs, labels = data
        # 将图像放入网络中进行运算，得到预测的网络输出outputs
        outputs = net(inputs)
        # 将梯度置零
        optimizer.zero_grad()
        # 计算预测的outputs与真实的标签labels之间的loss
        loss = Loss_function(outputs, labels)
        # 将loss进行反向传播计算
        loss.backward()
        # 更新梯度
        optimizer.step()

        # 每迭代1000次输出一次loss
        if (i+1) % 1000 == 0:
            print('epoch is %d, itration is %d, loss is %f' %
                  (epoch+1, i+1, loss.item()))
# 保存模型
torch.save(net, 'model.pkl')



"""
模型测试部分
"""
# 调用模型初始化网络
net_test = torch.load('model.pkl')

# 调用之前的test_loader迭代器
for i, data, in enumerate(test_loader, 0):
    # inputs代表图像，labels代表对应的标签
    inputs, labels = data
    # 将图像放入网络中进行运算，得到预测的网络输出outputs
    outputs = net_test(inputs)
    # outputs维数为（batch_size,10）即有batch_size每张图对应一个10维向量，分别代表归属10个类别的概率
    # 使用max函数对第1维（下标从0开始）求max，代表最大的类别归属概率
    _, predicts = torch.max(outputs, 1)

    predicts = predicts.numpy()
    labels = labels.numpy()

    # 输出当前batch中的10幅图
    for j in range(10):
        # 将tensor类型数据转为PIL图像格式数据
        image = inputs[j]*0.5+0.5  # 反归一化用于显示
        image = transforms.ToPILImage()(image)
        # 重采样放大图像
        image = image.resize((100, 100))
        # 从类别字典中获取数值与类别名称的映射关系
        predict = classes_dict[predicts[j]]
        label = classes_dict[labels[j]]
        # 输出图像，预测类别以及真实类别
        filename = 'num' + str(j+1) + '_predict_' + \
            predict + '_label_' + label + '.png'
        image.save(filename)
    break
