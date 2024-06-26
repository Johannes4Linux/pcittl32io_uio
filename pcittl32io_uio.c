#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/uio_driver.h>

#define PCITTL32IO_ID 0x3301
#define QUANCOM_ID 0x8008
#define IRQ_GEN 0xf9

static struct pci_device_id pcittl32io_ids[] = {
	{PCI_DEVICE(QUANCOM_ID, PCITTL32IO_ID)},
	{ }
};
MODULE_DEVICE_TABLE(pci, pcittl32io_ids);

static int pcittl32io_irq_ctrl(struct uio_info *info, s32 irq_en)
{
	u8 val;

	val = ioread8(info->mem[0].internal_addr + IRQ_GEN);
	val &= ~1;
	val |= (irq_en & 1);

	iowrite8(val, info->mem[0].internal_addr + IRQ_GEN);

	dev_info((&info->uio_dev->dev)->parent, "IRQ is %s\n", irq_en ? "enabled" : "disabled");
	return 0;
}

static irqreturn_t pcittl32io_irq_handler(int irq_nr, struct uio_info *info)
{
	pcittl32io_irq_ctrl(info, 0);
	dev_info((&info->uio_dev->dev)->parent, "IRQ was asserted\n");
	return IRQ_HANDLED;
}


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

	/* Add interrupt functions */
	info->irq = pdev->irq;
	info->handler = pcittl32io_irq_handler;
	info->irqcontrol = pcittl32io_irq_ctrl;

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
