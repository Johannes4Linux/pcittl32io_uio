#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/uio_driver.h>

#define PCITTL32IO_ID 0x3301
#define QUANCOM_ID 0x8008

static struct pci_device_id pcittl32io_ids[] = {
	{PCI_DEVICE(QUANCOM_ID, PCITTL32IO_ID)},
	{ }
};
MODULE_DEVICE_TABLE(pci, pcittl32io_ids);

static int pcittl32io_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int status;
	struct uio_info *info;
	resource_size_t start = pci_resource_start(pdev, 0);
	resource_size_t len = pci_resource_len(pdev, 0);

	dev_info(&pdev->dev, "Probing...\n");

	info = devm_kzalloc(&pdev->dev, sizeof(struct uio_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	status = pcim_enable_device(pdev);
	if (status) {
		dev_err(&pdev->dev, "Error enabling device\n");
		return status;
	}
	status = pci_request_regions(pdev, "pcittl32io");
	if (status) 
		goto free_dev;

	info->name = "pcittl32io";
	info->version = "V1.0";

	/* Let's setup memory entry for BAR0 */
	info->mem[0].name = "pcittl32io_bar0";
	info->mem[0].addr = start & PAGE_MASK;
	info->mem[0].offs = start & ~PAGE_MASK;
	info->mem[0].size = ((start & ~PAGE_MASK) + len + PAGE_SIZE -1) & PAGE_MASK;
	info->mem[0].memtype = UIO_MEM_PHYS;
	info->mem[0].internal_addr = pci_ioremap_bar(pdev, 0);

	if (!info->mem[0].internal_addr) {
		dev_err(&pdev->dev, "Error! No memory behind BAR0\n");
		status = -ENODEV;
		goto free_region;
	}


	status = uio_register_device(&pdev->dev, info);
	if (status) {
		dev_err(&pdev->dev, "Error registering UIO device\n");
		goto free_mem;
	}

	pci_set_drvdata(pdev, info);
	
	return 0;
free_mem:
	iounmap(info->mem[0].internal_addr);
free_region:
	pci_release_regions(pdev);
free_dev:
	pci_disable_device(pdev);
	return status;
}

static void pcittl32io_remove(struct pci_dev *pdev)
{
	struct uio_info *info;

	dev_info(&pdev->dev, "Removing...\n");

	info = pci_get_drvdata(pdev);

	iounmap(info->mem[0].internal_addr);
	uio_unregister_device(info);

	pci_release_regions(pdev);
	pci_disable_device(pdev);
}

static struct pci_driver pcittl32io_driver = {
	.name="pcittl32io_uio",
	.probe = pcittl32io_probe,
	.remove = pcittl32io_remove,
	.id_table = pcittl32io_ids
};

module_pci_driver(pcittl32io_driver);

MODULE_LICENSE("GPL");
