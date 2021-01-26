sudo amazon-linux-extras install aws-nitro-enclaves-cli -y
sudo yum install aws-nitro-enclaves-cli-devel -y
sudo usermod -aG ne $USER
sudo usermod -aG docker $USER
nitro-cli --version
sudo systemctl start nitro-enclaves-allocator.service && sudo systemctl enable nitro-enclaves-allocator.service
sudo systemctl start docker && sudo systemctl enable docker
