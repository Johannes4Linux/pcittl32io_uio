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

	status = uio_register_device(&pdev->dev, info);
	if (status) {
		dev_err(&pdev->dev, "Error registering UIO device\n");
		goto free_region;
	}

	pci_set_drvdata(pdev, info);
	
	return 0;

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
